#include "PDT.h"
#include "CFG.h"
#include "DFTree.h"
#include <set>

using namespace lemon;
using namespace std;

PDT::PDT (CFG &cfg, DFTree &t)
{
  m_label = string();
  m_cfg   = &cfg;
  m_tree  = &t;
  m_graph = new ListDigraph();
  
  // Step 2:
  /*
  set<ListDigraph::Node>::iterator v;
  for (int i = count -1; i > 0; --i)
    {
      ListDigraph::Node w = (*m_vertex)[i];
      v = (*m_pred)[w].begin();
      for (; v != (*m_pred)[w].end(); ++v);
	{
	  ListDigraph::Node u = eval (*v);
	  if ((*m_semi)[u] < (*m_semi)[w])
	    (*m_semi)[w] = (*m_semi)[u];
	}
      (*m_bucket)[(*m_vertex)[(*m_semi)[w]]].insert (w);
      (*m_ancestor)[w] = (*m_parent)[w]; // link
      
      /-*
      // Step 3:
      v = (*m_bucket)[(*m_parent)[w]].begin();
      for (; v != (*m_bucket)[(*m_parent)[w]].end(); ++v)
	{
	  (*m_bucket)[(*m_parent)[w]].erase (v);
	  ListDigraph::Node u = eval(*v);
	  if ((*m_semi)[u] < (*m_semi)[*v])
	    (*m_idom)[*v] = u;
	  else
	    (*m_idom)[*v] = (*m_parent)[w];
	}
      *-/
    }
  */
  /*
  // Step 4:
  for (int i = 1; i < count ; ++i)
    {
      Node w = (*m_vertex)[i];
      if ((*m_order)[(*m_idom)[w]] < (*m_semi)[w])
	(*m_idom)[w] = (*m_idom)[(*m_idom)[w]];
    }

  ArcIt a (*m_graph);
  for (; a != INVALID; ++a)
    m_graph -> erase (a);

  NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    if (n != m_cfg -> m_exit)
      m_graph -> addArc ((*m_idom)[n], n);
  */
}

ListDigraph::Node
PDT::eval (ListDigraph::Node v)
{
  /*
  ListDigraph::Node u = v;
  while ((*m_ancestor)[v] != INVALID)
    {
      if ((*m_semi)[v] < (*m_semi)[u])
	u = v;      
      v = (*m_ancestor)[v];
    }
  
  return u;
  */
  return v;
}
