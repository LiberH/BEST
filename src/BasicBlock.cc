#include "BasicBlock.h"
#include "Instruction.h"

using namespace std;
using namespace lemon;

BasicBlock::BasicBlock()
{
  m_id     = INVALID;
  m_name   = string();
  m_label  = string();
  
  m_graph  = new Graph ();
  m_instrs = new InstrMap (*m_graph, NULL);
  m_leader = INVALID;
  
  m_call   = NULL;
  m_ret    = INVALID;
}

Node
BasicBlock::addNode (Instruction &i)
{
  Node n = m_graph -> addNode();
  (*m_instrs)[n] = &i;
  i.m_id = n;

  ostringstream oss;
  oss << "I" << m_graph -> id (i.m_id);
  i.m_label = oss.str();
  i.m_name  = m_label + i.m_label;
  
  return i.m_id;
}

Arc
BasicBlock::addEdge (Node &n, Node &m)
{  
  return m_graph -> addArc (n, m);
}
