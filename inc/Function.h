#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "types.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class BasicBlock;
typedef ListDigraph::NodeMap<BasicBlock *> BBMap;

class Function {
  friend class CFG;
  friend class DFS;
  friend class Dot;
  
public:
  Function();
  Node addNode (BasicBlock &);
  Arc  addEdge (Node &, Node &);
  BasicBlock *findNode (string &);

  Node &entry () { return m_entry; };
  Node &exit  () { return m_exit; };

  void entry (Node &n) { m_entry = n; };
  void exit  (Node &n) { m_exit  = n; };

protected:
  Node    m_id;
  string  m_label;
  string  m_name;
  
  Graph  *m_graph;
  BBMap  *m_bbs;
  Node    m_entry;
  Node    m_exit;
};

#endif // _FUNCTION_H_
