#include "Slicer.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "PDG.hpp"
#include <lemon/list_graph.h>
#include <set>
#include <bitset>

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

Slicer::Slicer (CFG *cfg, PDG *pdg)
{
  m_cfg = cfg;
  m_pdg = pdg;
}

vector<Inst *> *
Slicer::slice ()
{
  vector<Inst *> *to_slice = new vector<Inst *> ();
  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*m_cfg -> m_bbs)[n];
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it = insts -> begin ();  
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  if (inst -> m_branch && !inst -> m_uncond)
	    to_slice -> push_back (inst);
	}
    }
  
  return this -> slice (to_slice);
}

vector<Inst *> *
Slicer::slice (u32 addr)
{
  ListDigraph::Node n = (*m_pdg -> m_nodes)[addr +1];
  Inst *inst = (*m_pdg -> m_insts)[n];
  
  vector<Inst *> *to_slice = new vector<Inst *> ();
  to_slice -> push_back (inst);
  return this -> slice (to_slice);
}
  
vector<Inst *> *
Slicer::slice (vector<Inst *> *insts)
{
  set<ListDigraph::Node> w;
  vector<Inst *> *slice = new vector<Inst *> ();
  vector<Inst *>::iterator inst_it = insts -> begin ();  
  ListDigraph::NodeMap<bool> marked (*m_pdg -> m_graph, false);
  for (; inst_it != insts -> end (); ++inst_it)
    {
      ListDigraph::Node n = (*m_pdg -> m_nodes)[(*inst_it) -> m_addr +1];
      Inst *inst = (*m_pdg -> m_insts)[n];
      slice -> push_back (inst);
      w.insert (n);
      marked[n] = true;
    }
  
  while (!w.empty ())
    {
      ListDigraph::Node n = *w.begin();
      w.erase (w.begin());
      
      ListDigraph::InArcIt ia_it (*m_pdg -> m_graph, n);
      for (; ia_it != INVALID; ++ia_it)
	{
	  ListDigraph::Arc a = ia_it;
	  ListDigraph::Node m = m_pdg -> m_graph -> source (a);
	  if ((*m_pdg -> m_meta)[n]
	  &&  (*m_pdg -> m_meta)[m])
	    {
	      Inst *inst = (*m_pdg -> m_insts)[m];
	      if (!marked[m])
		{
		  slice -> push_back (inst);
		  w.insert (m);
		  marked[m] = true;
		}
	    }

	  if ((*m_pdg -> m_meta)[n]
          && !(*m_pdg -> m_meta)[m])
	    {
	      if (!marked[m])
		{
		  w.insert (m);
		  marked[m] = true;
		}
	    }

	  if (!(*m_pdg -> m_meta)[n]
          &&  !(*m_pdg -> m_meta)[m])
	    {
	      BB *bb = (*m_pdg -> m_bbs)[m];
	      Inst *inst = bb -> m_insts -> back ();
	      ListDigraph::Node m = (*m_pdg -> m_nodes)[inst -> m_addr +1];
	      if (!marked[m])
		{
		  slice -> push_back (inst);
		  w.insert (m);
		  marked[m] = true;
		}	      
	    }	  
	}
    }

  return slice;
}

// static
vector<Inst *> *
Slicer::MinimizeSlice (vector<Inst *> *slice)
{
  vector<Inst *> *insts = new vector<Inst *> ();

  u64 refs = 0;
  u64 defs = 0;
  u64 mask = 0x80; // remove pc (implicit)
  vector<Inst *>::iterator slice_it = slice -> begin ();  
  for (; slice_it != slice -> end (); ++slice_it)
    {
      Inst *slice = *slice_it;
      Inst *inst  = new Inst (*slice);
      insts -> push_back (inst);
      
      refs |= inst -> m_refs;
      defs |= inst -> m_defs;
    }

  bitset<64> bs_refs (refs);
  bitset<64> bs_defs (defs);
  for (int b = 0; b < 64; ++b)
    if (bs_refs[b] && !bs_defs[b])
      mask |= 1 << b; // remove used by never set registers
  
  vector<Inst *>::iterator inst_it = insts -> begin ();  
  for (; inst_it != insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;      
      inst -> m_refs &= ~mask;
      inst -> m_defs &= ~mask;
    }
  
  return insts;
}
