#include "CFG.hpp"

#include "arch.h"
#include "Inst.hpp"
#include "BB.hpp"

#include <cstdio>
#include <graphviz/cgraph.h>
#include <lemon/maps.h>
#include <vector>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

/* PUBLIC: */

CFG::CFG ()
{
  ostringstream ss_name, ss_label;
  ss_name  << "cfg";
  ss_label << "CFG";

  m_name  = ss_name.str ();
  m_label = ss_label.str ();
  
  m_graph = new ListDigraph ();
  m_nodes = new map<int, ListDigraph::Node> ();
  m_bbs   = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  m_preds = new ListDigraph::NodeMap< vector<BB *> *> (*m_graph, NULL);
  m_succs = new ListDigraph::NodeMap< vector<BB *> *> (*m_graph, NULL);

  m_entry = INVALID;
  m_exit  = INVALID;
}

vector<BB *> *
CFG::bbs ()
{
  vector<BB *> *bbs = new vector<BB *> ();
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*m_bbs)[n];
      bbs -> push_back (bb);
    }
  
  return bbs;
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

// static
CFG *
CFG::Reverse (const CFG *cfg)
{
  CFG *rcfg = new CFG ();
  rcfg -> m_name  = "REV"  + cfg -> m_name;
  rcfg -> m_label = "Rev(" + cfg -> m_label + ")";
  
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      ListDigraph::Node m = rcfg -> m_graph -> addNode ();
      (*rcfg -> m_bbs)[m] = bb;  
      (*rcfg -> m_nodes)[bb -> m_entry] = m;
    }
  
  ListDigraph::ArcIt a (*cfg -> m_graph);
  for (; a != INVALID; ++a)
    {
      BB *bb = (*cfg -> m_bbs)[cfg -> m_graph -> source (a)];
      BB *cc = (*cfg -> m_bbs)[cfg -> m_graph -> target (a)];
      rcfg -> addEdge (*cc, *bb); // reversed
    }

  BB *bb = (*cfg -> m_bbs)[cfg -> m_entry];
  BB *cc = (*cfg -> m_bbs)[cfg -> m_exit];
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
  
  cfg -> findSuccs (*bbs);
  bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;
      ListDigraph::Node n = (*cfg -> m_nodes)[bb -> m_entry];
      vector<BB *>::iterator succ_it = (*cfg -> m_succs)[n] -> begin ();
      for (; succ_it != (*cfg -> m_succs)[n] -> end (); ++succ_it)
	{
	  BB *succ = *succ_it;
	  cfg -> addEdge (*bb, *succ);
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

// static
void
CFG::ToFile (string fn, CFG *cfg)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(cfg -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C(cfg -> m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);

  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
    }
  
  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }
  
  agwrite (agraph, f);
  agclose (agraph);
  fclose (f);
}

/* PRIVATE: */

void
CFG::findSuccs (vector<BB *> &bbs)
{
  ListDigraph::NodeIt m (*m_graph);
  for (; m != INVALID; ++m)
    {
      (*m_preds)[m] = new vector<BB *> ();
      (*m_succs)[m] = new vector<BB *> ();
    }
  
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
	  (*m_preds)[m] -> push_back (bb);
	  BB *bb = (*m_bbs)[m];
	  (*m_succs)[n] -> push_back (bb);
	  if (!inst -> m_uncond)
	    if (inst -> m_next)
	      {
		ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
		(*m_preds)[m] -> push_back (bb);
		BB *bb = (*m_bbs)[m];
		(*m_succs)[n] -> push_back (bb);
	      }
	}
      else
	if (inst -> m_next)
	  {
	    ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
	    (*m_preds)[m] -> push_back (bb);
	    BB *bb = (*m_bbs)[m];
	    (*m_succs)[n] -> push_back (bb);
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
	  Inst *last = bb -> m_insts -> back ();
	  Inst *first = cc -> m_insts -> front ();
	  last -> m_next = first;
	  addEdge (*bb, *cc);
	  w.push_back (lr);

	  (*m_preds)[lr] -> push_back (bb);
	  (*m_succs)[n] -> push_back (cc);
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
