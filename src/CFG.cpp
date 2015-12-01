#include "CFG.hpp"
#include "BB.hpp"

using namespace lemon;
using namespace std;

CFG::CFG (string label)
{
  m_id    = INVALID;
  m_name  = string ();
  m_label = label;
  
  m_graph    = new ListDigraph ();
  m_bbs      = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  m_entry    = INVALID;
  m_exit     = INVALID;
}

ListDigraph::Node
CFG::addNode (BB &bb)
{
  ListDigraph::Node n = m_graph -> addNode ();
  (*m_bbs)[n] = &bb;
  bb.m_id = n;

  bb.m_name = m_label + bb.m_label;
  return bb.m_id;
}

ListDigraph::Arc
CFG::addEdge (BB &bb,
	      BB &cc)
{
  ListDigraph::Node n = bb.m_id;
  ListDigraph::Node m = cc.m_id;
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
