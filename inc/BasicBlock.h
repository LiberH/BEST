#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "types.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Function;
class Instruction;
typedef ListDigraph::NodeMap<Instruction *> InstrMap;

class BasicBlock {
  friend class Function;
  friend class Dot;
  
public:
  BasicBlock();
  Node addNode (Instruction &);
  Arc  addEdge (Node &, Node &);
  
  Node     &leader () { return m_leader; };
  Function *call   () { return m_call;   };
  Node     &ret    () { return m_ret;    };
  
  void leader (Node     &n) { m_leader = n; };
  void call   (Function *f) { m_call   = f; };
  void ret    (Node     &n) { m_ret    = n; };

protected:
  Node      m_id;
  string    m_name;
  string    m_label;
  
  Graph    *m_graph;
  InstrMap *m_instrs;
  Node      m_leader;  
  Function *m_call;
  Node      m_ret;
};

#endif // _BASICBLOCK_H_
