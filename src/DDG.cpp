#include "DDG.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"

#include <cstdio>
#include <graphviz/cgraph.h>
#include <vector>
#include <bitset>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

static
string reg_names[] = {
  "cr"      , "ctr"   , "l1csr0" , "l1csr1" , "l1finv1" , "lr"   , "msr"  , "pc"   ,
  "serial0" , "srr0"  , "srr1"   , "xer"    , "hit"     , "miss" , "fpr0" , "fpr1" ,
  "fpr2"    , "fpr3"  , "fpr4"   , "fpr5"   , "fpr6"    , "fpr7" , "fpr8" , "fpr9" ,
  "fpr10"   , "fpr11" , "fpr12"  , "fpr13"  , "fpr14"  , "fpr15" ,
  
  "r0"  , "r1"  , "r2"  , "r3"  , "r4"  , "r5"  , "r6"  , "r7"  ,
  "r8"  , "r9"  , "r10" , "r11" , "r12" , "r13" , "r14" , "r15" ,
  "r16" , "r17" , "r18" , "r19" , "r20" , "r21" , "r22" , "r23" ,
  "r24" , "r25" , "r26" , "r27" , "r28" , "r29" , "r30" , "r31" };

/* PUBLIC */

DDG::DDG (CFG *cfg)
{  
  vector<Inst *> *insts = new vector<Inst *> ();
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();
      for (; inst_it != bb -> m_insts -> end (); ++inst_it)
	{
	  // TODO: retreivable form Inst::FromFile function.
	  Inst *inst = *inst_it;
	  insts -> push_back (inst);
	}
    }
  
  m_kills = new vector< set<Inst *> * > (64, NULL);
  for (int b = 0; b < 64; ++b)
    (*m_kills)[b] = new set<Inst *> ();

  int size = insts -> size ();
  m_gens = new map< int, set<Inst *> * > ();
  m_ins  = new map< int, set<Inst *> * > ();
  m_outs = new map< int, set<Inst *> * > ();
  m_deps = new map< int, set<Inst *> * > ();
  for (int i = 0; i < size; ++i)
    {
      Inst *inst = (*insts)[i];
      (*m_gens)[inst -> m_addr] = new set<Inst *> ();
      (*m_ins )[inst -> m_addr] = new set<Inst *> ();
      (*m_outs)[inst -> m_addr] = new set<Inst *> ();
      (*m_deps)[inst -> m_addr] = new set<Inst *> ();

      bitset<64> bs (inst -> m_defs);
      for (int b = 0; b < 64; ++b)
	if (bs[b])
	  {
	    (*m_kills)[b] -> insert (inst);
	    (*m_gens )[inst -> m_addr] -> insert (inst);
	  }
    }  
  
  vector<Inst *> w;
  set<Inst *>::iterator it;
  w.push_back ((*cfg -> m_bbs)[cfg -> m_entry] -> m_insts -> front ());
  while (!w.empty ())
    {
      // pick and remove an inst from the worklist.
      Inst *inst = w.front ();
      w.erase (w.begin ());
      
      // assign 'OUT' to 'OLDOUT'.
      set<Inst *> *oldout = new set<Inst *> ();
      set<Inst *>::iterator it = (*m_outs)[inst -> m_addr] -> begin ();
      for (; it != (*m_outs)[inst -> m_addr] -> end (); ++it)
	oldout -> insert (*it);

      // compute 'IN'.
      (*m_ins)[inst -> m_addr] -> clear ();
      ListDigraph::Node n = (*cfg -> m_nodes)[inst -> m_addr];
      BB *bb = (*cfg -> m_bbs)[n];
      Inst *bb_entry = bb -> m_insts -> front ();
      if (inst -> m_addr != bb_entry -> m_addr)
	{
	  Inst *prev = inst -> m_prev;
	  set<Inst *>::iterator it = (*m_outs)[prev -> m_addr] -> begin ();
	  for (; it != (*m_outs)[prev -> m_addr] -> end (); ++it)
	    (*m_ins)[inst -> m_addr] -> insert (*it);
	}
      else
	{
	  vector<BB *> *preds = (*cfg -> m_preds)[n];
	  vector<BB *>::iterator pred_it = preds -> begin ();
	  for (; pred_it != preds -> end (); ++pred_it)
	    {
	      BB *bb = *pred_it;
	      Inst *bb_exit = bb -> m_insts -> back ();
	      set<Inst *>::iterator it = (*m_outs)[bb_exit -> m_addr] -> begin ();
	      for (; it != (*m_outs)[bb_exit -> m_addr] -> end (); ++it)
		(*m_ins)[inst -> m_addr] -> insert (*it);
	    }
	}
      
      // compute 'KILL'.
      set<Inst *> *kill = new set<Inst *> ();
      it = (*m_gens)[inst -> m_addr] -> begin ();
      for (; it != (*m_gens)[inst -> m_addr] -> end (); ++it)
	for (int b = 0; b < 64; ++b)
	  {
	    Inst *inst = *it;
	    bitset<64> bs (inst -> m_defs);
	    if (bs[b])
	      kill -> insert ((*m_kills)[b] -> begin (),
		              (*m_kills)[b] -> end ());
	  }
      
      // compute 'OUT'.
      (*m_outs)[inst -> m_addr] -> clear ();
      
      it = (*m_ins)[inst -> m_addr] -> begin ();
      for (; it != (*m_ins)[inst -> m_addr] -> end (); ++it)
	(*m_outs)[inst -> m_addr] -> insert (*it);

      it = kill -> begin ();
      for (; it != kill -> end (); ++it)
	(*m_outs)[inst -> m_addr] -> erase (*it);
      
      it = (*m_gens)[inst -> m_addr] -> begin ();
      for (; it != (*m_gens)[inst -> m_addr] -> end (); ++it)
	(*m_outs)[inst -> m_addr] -> insert (*it);

      // check if 'OLDOUT' equals 'OUT'.
      if ((*oldout) != (*(*m_outs)[inst -> m_addr]))
	{
	  // add all successors to the worklist.
	  // TODO
	  ListDigraph::Node n = (*cfg -> m_nodes)[inst -> m_addr];
	  BB *bb = (*cfg -> m_bbs)[n];
	  Inst *bb_exit = bb -> m_insts -> back ();
	  if (inst -> m_addr != bb_exit -> m_addr)
	    {
	      w.push_back (inst -> m_next);
	    }
	  else
	    {
	      vector<BB *> *succs = (*cfg -> m_succs)[n];
	      vector<BB *>::iterator succ_it = succs -> begin ();
	      for (; succ_it != succs -> end (); ++succ_it)
		{
		  BB *bb = *succ_it;
		  Inst *bb_entry = bb -> m_insts -> front ();
		  w.push_back (bb_entry);
		}
	    }
	}
    }

  for (int i = 0; i < size; ++i)
    {
      Inst *inst = (*insts)[i];
      bitset<64> bs (inst -> m_refs);
      // TODO: which registers to filter?
      cout << hex << inst -> m_addr << ": " << inst -> m_disass << endl;
      for (int b = 0; b < 64; ++b)
	if (b != 7 && bs[b])
	  {
	    set<Inst *> *ins = (*m_ins)[inst -> m_addr];
	    set<Inst *>::iterator in_it = ins -> begin ();
	    for (; in_it != ins -> end (); ++in_it)
	      {
		Inst *in = *in_it;
		bitset<64> bs (in -> m_defs);
		if (bs[b])
		  {
		    cout << "  " << reg_names[b] << " - "
			 << hex << in -> m_addr << ": " << in -> m_disass << endl;
		    (*m_deps)[inst -> m_addr] -> insert (in);
		  }
	      }
	  }
    }
}

// static
void
DDG::ToFile (string fn, CFG *cfg, DDG *ddg)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen ("DDG", Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C("DDG(" + cfg -> m_label + ")"));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg -> m_graph;
  map<int, Agnode_t *> *agnodes = new map<int, Agnode_t *> ();
  Agnode_t *agn = agnode (agraph, "start", TRUE);
  agsafeset (agn, "label", "_start", "error");

  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it  = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  ostringstream oss;
	  Inst *inst = *inst_it;
	  oss << hex << inst -> m_addr;
	  (*agnodes)[inst -> m_addr] = agnode (agraph, C(inst -> m_name), TRUE);
	  agsafeset ((*agnodes)[inst -> m_addr], "label", C(oss.str ()), "error");
	  agsafeset ((*agnodes)[inst -> m_addr], "ranksep", "0.2",   "");  
	  agsafeset ((*agnodes)[inst -> m_addr], "shape",   "box",   "");  
	  agsafeset ((*agnodes)[inst -> m_addr], "height",  "0.3",   "");
	}
    }
  
  ListDigraph::NodeIt m (*graph);
  for (; m != INVALID; ++m)
    {
      BB *bb = (*cfg -> m_bbs)[m];
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it  = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  set<Inst *> *deps = (*ddg -> m_deps)[inst -> m_addr];
	  if (deps -> empty ())
	      agedge (agraph, agn, (*agnodes)[inst -> m_addr], NULL, TRUE);
	  else
	    { 
	      set<Inst *>::iterator dep_it  = deps -> begin ();
	      for (; dep_it != deps -> end (); ++dep_it)
		{
		  Inst *dep = *dep_it;
		  agedge (agraph, (*agnodes)[dep -> m_addr], (*agnodes)[inst -> m_addr], NULL, TRUE);
		}
	    }
	}
    }

  agwrite (agraph, f);
  agclose (agraph);
  fclose (f);
}
