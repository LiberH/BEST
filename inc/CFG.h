#ifndef _CFG_H_
#define _CFG_H_

#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Function;
typedef ListDigraph::NodeMap<Function *> FunctionMap;

class CFG {
public:
  string       label_;
  ListDigraph *graph_;
  FunctionMap *functions_;

  
                     CFG         (string);
  ListDigraph::Node  addFunction (Function &);
  void               addCall     (Function &, string, string, Function &);
  Function          *findNode    (string);
};

#endif // _CFG_H_
