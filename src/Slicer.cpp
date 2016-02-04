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
  m_pdg = pdg;
}

vector<Inst *> *
Slicer::slice (u32 addr)
{
  vector<Inst *> *slice = new vector<Inst *> ();

  ListDigraph::Node n = (*m_pdg -> m_nodes)[addr +1];
  Inst *inst = (*m_pdg -> m_insts)[n];
  slice -> push_back (inst);

  ListDigraph::NodeMap<bool> marked (*m_pdg -> m_graph, false);
  set<ListDigraph::Node> w;
  w.insert (n);
  marked[n] = true;

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
