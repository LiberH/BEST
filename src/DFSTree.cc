#include "DFSTree.h"
#include "CFG.h"
#include <lemon/dfs.h>

using namespace lemon;
using namespace std;

DFSTree::DFSTree (const DFSTree &t)
{
  m_label = t.m_label;
  m_cfg   = t.m_cfg;
  m_ref   = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph);
  
  int count = countNodes (*m_cfg -> m_graph);
  m_list  = new vector<ListDigraph::Node> (count, INVALID);
  m_graph = new ListDigraph ();

  int i = 0;
  m_order  = new ListDigraph::NodeMap<int> (*m_cfg -> m_graph, -1);
  m_parent = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph, INVALID);
  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n, ++i)
    {
      (*m_ref   )[n] = m_graph -> addNode ();
      (*m_list  )[i] = (*t.m_list)[i];
      (*m_order )[n] = (*t.m_order)[n];
      (*m_parent)[n] = (*t.m_parent)[n];
    }

  m_root = (*m_ref)[m_cfg -> m_entry];
  ListDigraph::NodeIt m (*m_cfg -> m_graph);
  for (; m != INVALID; ++m)
    if ((*m_ref)[m] != m_root)
      m_graph -> addArc ((*m_ref)[(*m_parent)[m]], (*m_ref)[m]);
}

DFSTree::DFSTree (const CFG &cfg)
{
  m_label = string ();
  m_cfg   = &cfg;
  m_ref   = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph);

  int count = countNodes (*m_cfg -> m_graph);
  m_list  = new vector<ListDigraph::Node> (count, INVALID);
  m_graph = new ListDigraph ();
  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    (*m_ref)[n] = m_graph -> addNode ();

  Dfs<ListDigraph> dfs (*m_cfg -> m_graph);
  m_order  = new ListDigraph::NodeMap<int> (*m_cfg -> m_graph, -1);
  m_parent = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph, INVALID);    

  int i = 0;
  dfs.init ();
  dfs.addSource (m_cfg -> m_entry);
  (*m_list)[i] = m_cfg -> m_entry;
  (*m_order)[m_cfg -> m_entry] = i;
  (*m_parent)[m_cfg -> m_entry] = INVALID;
  while (!dfs.emptyQueue ())
    {
      ListDigraph::Arc  a = dfs.nextArc ();
      ListDigraph::Node u = m_cfg -> m_graph -> source (a);
      ListDigraph::Node v = m_cfg -> m_graph -> target (a);
      if (!dfs.reached (v))
	{
	  i++;
	  
	  (*m_list  )[i] = v;
	  (*m_order )[v] = i;
	  (*m_parent)[v] = u;
	}
      
      dfs.processNextArc ();
    }

  m_root = (*m_ref)[m_cfg -> m_entry];
  ListDigraph::ArcIt a (*m_cfg -> m_graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node u = m_cfg -> m_graph -> source (a);
      ListDigraph::Node v = m_cfg -> m_graph -> target (a);
      m_graph -> addArc ((*m_ref)[u], (*m_ref)[v]);
    }
}

DFSTree *
DFSTree::tree (const DFSTree &t)
{
  DFSTree *u = new DFSTree (t);
  return u;
}
