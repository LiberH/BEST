#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "CFG.h"
#include "BasicBlock.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class CFG;
class BasicBlock;
typedef ListDigraph::NodeMap<BasicBlock *> BasicBlockMap;

class Function {
public:
  ListDigraph::Node  id_;
  string             label_;
  string             name_;
  ListDigraph       *graph_;
  BasicBlockMap     *basicblocks_;
  ListDigraph::Node  entry_;
  ListDigraph::Node  exit_;
  
  
                     Function           (CFG &, string);
  ListDigraph::Node  addBasicBlock      (BasicBlock &);
  ListDigraph::Node  addEntry           (BasicBlock &);
  ListDigraph::Node  addExit            ();
  ListDigraph::Arc   addControlFlowEdge (ListDigraph::Node, ListDigraph::Node);
  BasicBlock        *findNode           (string);
};

#endif // _FUNCTION_H_
