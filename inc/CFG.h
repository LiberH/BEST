#ifndef _CFG_H_
#define _CFG_H_

#include "Function.h"
#include <graphviz/cgraph.h>
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Function;
class CFG {
public:
  typedef ListDigraph::NodeMap<Function *> FunctionMap;
  typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;

  std::string name_;
  ListDigraph *graph_;
  FunctionMap *functions_;
  
  Agraph_t *agraph_;
  AgnodeMap *agnodes_;
  
  CFG (string &);
  ListDigraph::Node addFunction (Function &);
  ListDigraph::Arc addCall (string &, string &, Function &, Function &);
};

#endif // _CFG_H_
