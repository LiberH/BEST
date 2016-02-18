#include "Slicer.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "PDG.hpp"
#include <lemon/list_graph.h>
#include <set>

using namespace std;
using namespace lemon;

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
	  if (inst -> m_branch)
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
