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
  m_ref      = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph);
  m_semi     = new ListDigraph::NodeMap<int> (*m_cfg -> m_graph);
  m_idom     = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph, INVALID);
  m_ancestor = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph, INVALID);
  m_bucket   = new ListDigraph::NodeMap< set<ListDigraph::Node> > (*m_cfg -> m_graph, set<ListDigraph::Node> ());
  
  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      (*m_ref )[n] = m_graph -> addNode ();
      (*m_semi)[n] = (*m_tree -> m_order)[n];
    }
  
  // Step 2: Iteratively find the semi-dominators
  set<ListDigraph::Node>::iterator v;
  int count = countNodes (*m_cfg -> m_graph);
  for (int i = count -1; i > 0; --i)
    {
      ListDigraph::Node w = (*m_tree -> m_list)[i];
      ListDigraph::InArcIt a (*m_cfg -> m_graph, w);
      for (; a != INVALID; ++a)
	{
	  ListDigraph::Node v = m_cfg -> m_graph -> source (a);
	  ListDigraph::Node u = eval (v);
	  if ((*m_semi)[u] < (*m_semi)[w])
	    (*m_semi)[w] = (*m_semi)[u];
	}

      // add w to bucket(vertex(semi(w)))
      BB *bb = (*m_cfg -> m_bbs)[w];
      BB *sm = (*m_cfg -> m_bbs)[(*m_tree -> m_list)[(*m_semi)[w]]];
      cout << "w        = " << bb -> m_label << endl;
      cout << "semi(w) := " << sm -> m_label << endl;
      cout << "add w to bucket(semi(w))" << endl;
      
      (*m_bucket)[(*m_tree -> m_list)[(*m_semi)[w]]].insert (w);
      (*m_ancestor)[w] = (*m_tree -> m_parent)[w]; // link

      // Step 3:
      BB *pr = (*m_cfg -> m_bbs)[(*m_tree -> m_parent)[w]];
      cout << "parent(w) = " << pr -> m_label << endl;
      cout << "bucket(parent(w)) = {";
      v = (*m_bucket)[(*m_tree -> m_parent)[w]].begin();
      for (; v != (*m_bucket)[(*m_tree -> m_parent)[w]].end(); ++v)
	{
	  BB *bb = (*m_cfg -> m_bbs)[*v];
	  cout << bb -> m_label;
	  //(*m_bucket)[(*m_tree -> m_parent)[w]].erase (v);
	  ListDigraph::Node u = eval(*v);
	  if ((*m_semi)[u] < (*m_semi)[*v])
	    (*m_idom)[*v] = u;
	  else
	    (*m_idom)[*v] = (*m_tree -> m_parent)[w];
	}
      cout << "}" << endl << endl;
    }
  
  // Step 4:
  for (int i = 0; i < count ; ++i)
    {
      ListDigraph::Node w = (*m_tree -> m_list)[i];
      if ((*m_tree -> m_order)[(*m_idom)[w]] != (*m_semi)[w])
	(*m_idom)[w] = (*m_idom)[(*m_idom)[w]];
    }

  ListDigraph::NodeIt m (*m_graph);
  for (; m != INVALID; ++m)
    //if (m != m_tree -> m_root)
      m_graph -> addArc ((*m_idom)[m], m);
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
