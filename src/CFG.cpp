#include "CFG.hpp"

#include "arch.h"
#include "Inst.hpp"
#include "BB.hpp"
#include <lemon/maps.h>
#include <vector>

using namespace std;
using namespace lemon;

/* PRIVATE: */

// static
int CFG::m_id = 0;

void
CFG::findTargets (vector<BB *> &bbs)
{
  ListDigraph::NodeIt m (*m_graph);
  for (; m != INVALID; ++m)
      (*m_targets)[m] = new vector<BB *> ();

  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      BB   *bb   = (*m_bbs)[n];
      Inst *inst = bb -> m_insts -> back ();

      // TODO: hacky ; to clean
      if (inst -> m_disass == "bclr- 0,lt")
	continue;
      
      if (inst -> m_branch)
	{
	  ListDigraph::Node m = (*m_nodes)[inst -> m_target];
	  BB *bb = (*m_bbs)[m];
	  (*m_targets)[n] -> push_back (bb);
	  if (!inst -> m_uncond)
	    if (inst -> m_next)
	      {
		ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
		BB *bb = (*m_bbs)[m];
		(*m_targets)[n] -> push_back (bb);
	      }
	}
      else
	if (inst -> m_next)
	  {
	    ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
	    BB *bb = (*m_bbs)[m];
	    (*m_targets)[n] -> push_back (bb);
	  }
    }
}

void
CFG::blr_patch ()
{
  vector<ListDigraph::Node> w;
  vector<ListDigraph::Node> d;
  vector<ListDigraph::Node> r;
  ListDigraph::Node n = m_entry;
  w.push_back (n);

  ListDigraph::NodeMap<bool> mark (*m_graph, false);
  OutDegMap<ListDigraph> odm (*m_graph);
  while (!w.empty ())
    {
      n = w.back ();
      w.pop_back ();
      mark[n] = true;
      
      BB *bb = (*m_bbs)[n];
      if (odm[n] == 0)
	{
	  ListDigraph::Node lr = r.back ();
	  r.pop_back ();
	  
	  BB *cc = (*m_bbs)[lr];
	  addEdge (*bb, *cc);
	  w.push_back (lr);
	}
      else
	{
	  ListDigraph::Node m = INVALID;
	  ListDigraph::OutArcIt oait (*m_graph, n);
	  for (; oait != INVALID; ++oait)
	    {
	      m = m_graph -> target (oait);
	      if (!mark[m])
		w.push_back (m);
	    }

	  Inst *i = bb -> m_insts -> back ();
	  size_t found = i -> m_disass.find("bl ");
	  if (found != string::npos)
	    r.push_back ((*m_nodes)[i -> m_addr +4]);
	}
    }
}

/* PUBLIC: */

// static
CFG *
CFG::Reverse (const CFG &cfg)
{
  CFG *rcfg = new CFG ();
  rcfg -> m_name  = "REV"  + cfg.m_name;
  rcfg -> m_label = "Rev(" + cfg.m_label + ")";
  
  ListDigraph::NodeIt n (*cfg.m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg.m_bbs)[n];
      ListDigraph::Node m = rcfg -> m_graph -> addNode ();
      (*rcfg -> m_bbs)[m] = bb;  
      (*rcfg -> m_nodes)[bb -> m_entry] = m;
    }
  
  ListDigraph::ArcIt a (*cfg.m_graph);
  for (; a != INVALID; ++a)
    {
      BB *bb = (*cfg.m_bbs)[cfg.m_graph -> source (a)];
      BB *cc = (*cfg.m_bbs)[cfg.m_graph -> target (a)];
      rcfg -> addEdge (*cc, *bb); // reversed
    }

  BB *bb = (*cfg.m_bbs)[cfg.m_entry];
  BB *cc = (*cfg.m_bbs)[cfg.m_exit];
  rcfg -> m_entry = (*rcfg -> m_nodes)[cc -> m_entry]; // reversed
  rcfg -> m_exit  = (*rcfg -> m_nodes)[bb -> m_entry]; // id.
  
  return rcfg;
}

// static
CFG *
CFG::FromFile (string f)
{
  CFG          *cfg = new CFG ();
  vector<BB *> *bbs = BB::FromFile (f);
  vector<BB *>::iterator bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;
      cfg -> addBB (*bb);
    }
  
  cfg -> findTargets (*bbs);
  bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;
      ListDigraph::Node n = (*cfg -> m_nodes)[bb -> m_entry];
      vector<BB *>::iterator target_it = (*cfg -> m_targets)[n] -> begin ();
      for (; target_it != (*cfg -> m_targets)[n] -> end (); ++target_it)
	{
	  BB *target = *target_it;
	  cfg -> addEdge (*bb, *target);
	}
    }

  arch a;
  u32 entry_addr, exit_addr;
  a.readCodeFile (f.c_str ());
  a.getFunctionName ("_start", entry_addr);
  a.getFunctionName ("shouldNotHappen", exit_addr);
  cfg -> m_entry = (*cfg -> m_nodes)[entry_addr];  
  cfg -> m_exit  = (*cfg -> m_nodes)[exit_addr];

  cfg -> blr_patch ();  
  return cfg;
}

CFG::CFG ()
{
  const int id = m_id++;
  
  ostringstream ss_name, ss_label;
  ss_name  << "cfg" << id;
  ss_label << "CFG" << id;  

  m_name    = ss_name.str ();
  m_label   = ss_label.str ();
  
  m_graph   = new ListDigraph ();
  m_nodes   = new map<int, ListDigraph::Node> ();
  m_bbs     = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  m_targets = new ListDigraph::NodeMap< vector<BB *> *> (*m_graph, NULL);

  m_entry   = INVALID;
  m_exit    = INVALID;
}

ListDigraph::Node
CFG::addBB (BB &bb)
{
  ListDigraph::Node n = m_graph -> addNode ();
  bb.m_name = m_name + bb.m_name;
  
  (*m_bbs)[n] = &bb;
  vector<Inst *>::iterator inst_it = bb.m_insts -> begin ();
  for (; inst_it != bb.m_insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      (*m_nodes)[inst -> m_addr] = n;
    }
  
  return n;
}

ListDigraph::Arc
CFG::addEdge (BB &bb,
	      BB &cc)
{
  ListDigraph::Node n = (*m_nodes)[bb.m_entry];
  ListDigraph::Node m = (*m_nodes)[cc.m_entry];
  return m_graph -> addArc (n, m);
}
