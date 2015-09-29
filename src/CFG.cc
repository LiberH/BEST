#include "CFG.h"
#include "Function.h"

using namespace std;
using namespace lemon;

CFG::CFG(string &s)
{
  m_label  = s;
  
  m_graph  = new Graph ();
  m_functs = new FunctMap (*m_graph, NULL);
}

Node
CFG::addNode (Function &f)
{
  Node n = m_graph -> addNode();
  (*m_functs)[n] = &f;
  f.m_id = n;
  
  ostringstream oss;
  oss << "F" << m_graph -> id (f.m_id);
  f.m_label = oss.str();
  f.m_name = f.m_label;

  return f.m_id;
}

Function *
CFG::findNode (string &s)
{
  Function *f;
  NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      f = (*m_functs)[n];
      if (f -> m_label == s)
	break;
    }

  return f;
}
