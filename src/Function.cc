#include "Function.h"
#include "BasicBlock.h"

using namespace std;
using namespace lemon;

Function::Function()
{
  m_id    = INVALID;
  m_name  = string();
  m_label = string();
  
  m_graph = new Graph();
  m_bbs   = new BBMap (*m_graph, NULL);
  m_entry = INVALID;
  m_exit  = INVALID;
}

Node
Function::addNode (BasicBlock &bb)
{
  Node n = m_graph -> addNode();
  (*m_bbs)[n] = &bb;
  bb.m_id = n;

  ostringstream oss;
  oss << "BB" << m_graph -> id (bb.m_id);
  bb.m_label = oss.str();
  bb.m_name = m_label + bb.m_label;

  return bb.m_id;
}

Arc
Function::addEdge (Node &n, Node &m)
{
  return m_graph -> addArc (n, m);
}


BasicBlock *
Function::findNode (string &s)
{
  BasicBlock *bb;
  NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      bb = (*m_bbs)[n];
      if (bb -> m_label == s)
	break;
    }

  return bb;
}
