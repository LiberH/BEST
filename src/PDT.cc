#include "PDT.h"
#include "CFG.h"
#include "DFSTree.h"
#include "BB.h" // To remove
#include <set>

using namespace lemon;
using namespace std;

PDT::PDT (CFG &cfg, DFSTree &t) // Remove "CFG &cfg"
{
  m_label    = string();
  m_cfg      = &cfg;
  m_tree     = &t;
  m_graph    = new ListDigraph();
  m_semi     = new ListDigraph::NodeMap<int> (*m_cfg -> m_graph);
  m_ancestor = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph, INVALID);

  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    (*m_semi)[n] = (*m_tree -> m_order)[n];
  
  // Step 2:
  set<ListDigraph::Node>::iterator v;
  int count = countNodes (*m_cfg -> m_graph);
  for (int i = count -1; i > 0; --i)
    {
      ListDigraph::Node w = (*m_tree -> m_list)[i]; //(*m_vertex)[i];
      BB *bb = (*m_cfg -> m_bbs)[w];
      cout << bb -> m_label << endl;
      ListDigraph::InArcIt a (*m_cfg -> m_graph, w);
      //v = (*m_pred)[w].begin();
      for (; a != INVALID; ++a)
      //for (; v != (*m_pred)[w].end(); ++v);
	{
	  ListDigraph::Node v = m_cfg -> m_graph -> source (a);
	  ListDigraph::Node u = eval (v);
	  
	  BB *bb = (*m_cfg -> m_bbs)[v];
	  cout << "  " << bb -> m_label
	       << " " << (*m_semi)[u] << " < "
	       << " " << (*m_semi)[w] << endl;

	  if ((*m_semi)[u] < (*m_semi)[w])
	    (*m_semi)[w] = (*m_semi)[u];
	}

      //(*m_bucket)[(*m_vertex)[(*m_semi)[w]]].insert (w);
      (*m_ancestor)[w] = (*m_tree -> m_parent)[w]; // link
      
      /*
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
      */
      cout << endl;
    }

  /*
  // Step 4:
  for (int i = 1; i < count ; ++i)
    {
      Node w = (*m_vertex)[i];
      if ((*m_order)[(*m_idom)[w]] < (*m_semi)[w])
	(*m_idom)[w] = (*m_idom)[(*m_idom)[w]];
    }
  */
}

ListDigraph::Node
PDT::eval (ListDigraph::Node v)
{
  ListDigraph::Node u = v;
  while ((*m_ancestor)[v] != INVALID)
    {
      if ((*m_semi)[v] < (*m_semi)[u])
	u = v;
      
      v = (*m_ancestor)[v];
    }
  
  return u;
}
