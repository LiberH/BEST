#include "CFG.h"
#include "BB.h"
#include <lemon/dfs.h>

using namespace lemon;
using namespace std;

CFG::CFG ()
{
  m_id    = INVALID;
  m_name  = string ();
  m_label = string ();
  
  m_graph    = new ListDigraph ();
  m_bbs      = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  m_entry    = INVALID;
  m_exit     = INVALID;
}

CFG::CFG (const CFG &f)
{
  ListDigraph::NodeMap<ListDigraph::Node> ref (*f.m_graph);

  m_id    = f.m_id;
  m_name  = f.m_name;
  m_label = f.m_label;
  
  m_graph = new ListDigraph ();
  m_bbs   = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  ListDigraph::NodeIt n (*f.m_graph);
  for (; n != INVALID; ++n)
    {
      ref[n] = m_graph -> addNode ();
      (*m_bbs)[ref[n]] = (*f.m_bbs)[n];
    }
  
  ListDigraph::ArcIt a (*f.m_graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node u = f.m_graph -> source (a);
      ListDigraph::Node v = f.m_graph -> target (a);
      m_graph -> addArc (ref[u], ref[v]);
    }
  
  m_entry = ref[f.m_entry];
  m_exit  = ref[f.m_exit];
}

CFG *
CFG::reverse (const CFG &f)
{
  CFG *g = new CFG (f);
  ListDigraph::NodeMap<ListDigraph::Node> ref (*f.m_graph);

  g -> m_id    = f.m_id;
  g -> m_name  = f.m_name;
  g -> m_label = f.m_label;
  
  g -> m_graph = new ListDigraph ();
  g -> m_bbs   = new ListDigraph::NodeMap<BB *> (*g -> m_graph, NULL);
  ListDigraph::NodeIt n (*f.m_graph);
  for (; n != INVALID; ++n)
    {
      ref[n] = g -> m_graph -> addNode ();
      (*g -> m_bbs)[ref[n]] = (*f.m_bbs)[n];
    }
  
  ListDigraph::ArcIt a (*f.m_graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node u = f.m_graph -> source (a);
      ListDigraph::Node v = f.m_graph -> target (a);
      g -> m_graph -> addArc (ref[v], ref[u]);
    }
  
  g -> m_entry = ref[f.m_exit];
  g -> m_exit  = ref[f.m_entry];
  
  return g;
}

CFG *
CFG::DFS (const CFG &f)
{
  int i = 0;
  CFG *g = new CFG (f);
  Dfs<ListDigraph> dfs (*g -> m_graph);
  ListDigraph::NodeMap<ListDigraph::Node> parent (*g -> m_graph);
  
  ListDigraph::Node r = g -> m_entry;
  BB *bb = (*g -> m_bbs)[r];
  ostringstream oss;
  oss << i;
  bb -> m_label = oss.str ();
  
  dfs.init ();
  dfs.addSource (r);
  while (!dfs.emptyQueue ())
    {
      ListDigraph::Arc  a = dfs.nextArc ();
      ListDigraph::Node u = g -> m_graph -> source (a);
      ListDigraph::Node v = g -> m_graph -> target (a);
      if (!dfs.reached (v))
	{
	  ++i;

	  BB *bb = (*g -> m_bbs)[v];
	  ostringstream oss;
	  oss << i;
	  bb -> m_label = oss.str ();
	  parent[v] = u;
	}
      
      dfs.processNextArc ();
    }
  
  ListDigraph::ArcIt a (*g -> m_graph);
  for (; a != INVALID; ++a)
    g -> m_graph -> erase (a);

  ListDigraph::NodeIt n (*g -> m_graph);
  for (; n != INVALID; ++n)
    if (n != g -> m_entry)
      g -> m_graph -> addArc (parent[n], n);
  
  return g;
}

ListDigraph::Node
CFG::addNode (BB &bb)
{
  ListDigraph::Node n = m_graph -> addNode ();
  (*m_bbs)[n] = &bb;
  bb.m_id = n;

  ostringstream oss;
  oss << "BB" << m_graph -> id (bb.m_id);
  bb.m_label = oss.str ();
  bb.m_name = m_label + bb.m_label;

  return bb.m_id;
}

ListDigraph::Arc
CFG::addEdge (ListDigraph::Node &n,
	      ListDigraph::Node &m)
{
  return m_graph -> addArc (n, m);
}


BB *
CFG::findNode (string &s)
{
  BB *bb;
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      bb = (*m_bbs)[n];
      if (bb -> m_label == s)
	break;
    }

  return bb;
}
