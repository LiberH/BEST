#ifndef _CFG_H_
#define _CFG_H_

#include "types.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Function;
typedef ListDigraph::NodeMap<Function *> FunctMap;

class CFG {
  friend class Dot;
  
public:
  CFG (string &);
  Node addNode (Function &);  
  Function *findNode (string &);

protected:
  string    m_label;
  
  Graph    *m_graph;
  FunctMap *m_functs;
};

#endif // _CFG_H_
