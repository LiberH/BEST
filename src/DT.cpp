#include "DT.hpp"

#include "CFG.hpp"
#include "DFS.hpp"

using namespace std;
using namespace lemon;

DT::DT (const CFG *cfg)
{
  ostringstream ss_name, ss_label;
  ss_name  << "dt";
  ss_label << "DT(" << cfg -> m_label << ")";

  m_name  = ss_name.str ();
  m_label = ss_label.str ();

  m_cfg      = cfg;
  m_sdom     = new ListDigraph::NodeMap<int> (*cfg -> m_graph, 0);
  m_idom     = new ListDigraph::NodeMap<ListDigraph::Node> (*cfg -> m_graph, INVALID);
  m_ancestor = new ListDigraph::NodeMap<ListDigraph::Node> (*cfg -> m_graph, INVALID);
  m_bucket   = new ListDigraph::NodeMap< set<ListDigraph::Node> * > (*cfg -> m_graph, NULL);

  DFS *dfs = new DFS (*cfg);
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      (*m_sdom  )[n] = (*dfs -> m_order)[n];
      (*m_bucket)[n] = new set<ListDigraph::Node> ();
    }
  
  // Step 2: Iteratively find the semi-dominators
  set<ListDigraph::Node>::iterator v;
  int count = countNodes (*cfg -> m_graph);
  for (int i = count -1; i > 0; --i)
    {
      ListDigraph::Node w = (*dfs -> m_list)[i];
      if (w == INVALID) continue;
      
      ListDigraph::InArcIt a (*cfg -> m_graph, w);
      for (; a != INVALID; ++a)
	{
	  ListDigraph::Node v = cfg -> m_graph -> source (a);
	  ListDigraph::Node u = eval (v);
	  if ((*m_sdom)[u] < (*m_sdom)[w])
	    (*m_sdom)[w] = (*m_sdom)[u];
	}

      (*m_bucket)[(*dfs -> m_list)[(*m_sdom)[w]]] -> insert (w); // add w to bucket(vertex(semi(w)))
      (*m_ancestor)[w] = (*dfs -> m_parent)[w]; // link

      // Step 3:
      v = (*m_bucket)[(*dfs -> m_parent)[w]] -> begin();
      for (; v != (*m_bucket)[(*dfs -> m_parent)[w]] -> end(); ++v)
	{
	  (*m_bucket)[(*dfs -> m_parent)[w]] -> erase (v);
	  ListDigraph::Node u = eval(*v);
	  if ((*m_sdom)[u] < (*m_sdom)[*v])
	    (*m_idom)[*v] = u;
	  else
	    (*m_idom)[*v] = (*dfs -> m_parent)[w];
	}
    }
  
  // Step 4:
  for (int i = 0; i < count ; ++i)
    {
      ListDigraph::Node w = (*dfs -> m_list)[i];
      if ((*dfs -> m_order)[(*m_idom)[w]] != (*m_sdom)[w])
	(*m_idom)[w] = (*m_idom)[(*m_idom)[w]];
    }
}

ListDigraph::Node
DT::eval (ListDigraph::Node v)
{
  ListDigraph::Node u = v;
  while ((*m_ancestor)[v] != INVALID)
    {
      if ((*m_sdom)[v] < (*m_sdom)[u])
	u = v;
      
      v = (*m_ancestor)[v];
    }
  
  return u;
}
