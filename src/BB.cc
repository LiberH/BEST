#include "BB.h"
#include "Inst.h"

using namespace lemon;
using namespace std;

BB::BB()
{
  m_id     = INVALID;
  m_name   = string();
  m_label  = string();
  
  m_graph  = new ListDigraph ();
  m_instrs = new ListDigraph::NodeMap<Inst *> (*m_graph, NULL);
  m_leader = INVALID;  
  m_call   = NULL;
  m_ret    = INVALID;
}

ListDigraph::Node
BB::addNode (Inst &i)
{
  ListDigraph::Node n = m_graph -> addNode();
  (*m_instrs)[n] = &i;
  i.m_id = n;

  ostringstream oss;
  oss << "I" << m_graph -> id (i.m_id);
  i.m_label = oss.str();
  i.m_name  = m_label + i.m_label;
  
  return i.m_id;
}

ListDigraph::Arc
BB::addEdge (ListDigraph::Node &n,
	     ListDigraph::Node &m)
{  
  return m_graph -> addArc (n, m);
}
