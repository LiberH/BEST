#include "DFTree.h"
#include "CFG.h"
#include <lemon/dfs.h>

using namespace lemon;
using namespace std;

DFTree::DFTree (const CFG &cfg)
{
  m_label = string ();
  m_cfg = &cfg;
  m_ref = new ListDigraph::NodeMap<ListDigraph::Node> (*m_cfg -> m_graph);

  m_graph = new ListDigraph ();
  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    (*m_ref)[n] = m_graph -> addNode ();

  Dfs<ListDigraph> dfs (*m_cfg -> m_graph);
  ListDigraph::NodeMap<ListDigraph::Node> parent (*m_cfg -> m_graph, INVALID);    
  
  dfs.init ();
  dfs.addSource (m_cfg -> m_entry);
  while (!dfs.emptyQueue ())
    {
      ListDigraph::Arc  a = dfs.nextArc ();
      ListDigraph::Node u = m_cfg -> m_graph -> source (a);
      ListDigraph::Node v = m_cfg -> m_graph -> target (a);
      if (!dfs.reached (v))
	parent[v] = u;
      
      dfs.processNextArc ();
    }

  m_root = (*m_ref)[m_cfg -> m_entry];
  ListDigraph::NodeIt m (*m_cfg -> m_graph);
  for (; m != INVALID; ++m)
    if ((*m_ref)[m] != m_root)
      m_graph -> addArc ((*m_ref)[parent[m]], (*m_ref)[m]);
}
