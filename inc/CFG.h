#ifndef _CFG_H_
#define _CFG_H_

#include "Function.h"
#include <lemon/list_graph.h>
#include <graphviz/cgraph.h>

using namespace std;
using namespace lemon;

class Function;
typedef ListDigraph::NodeMap<Function *> FunctionMap;
typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;

class CFG {
public:
  string       name_;
  ListDigraph *graph_;
  FunctionMap *functions_;
  
  Agraph_t    *agraph_;
  AgnodeMap   *agnodes_;

  
                    CFG         (string &);
  ListDigraph::Node addFunction (Function &);
  void              addCall     (Function &, string &, string &, Function &);

  string            toDot       ();
};

#endif // _CFG_H_
