#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "BasicBlock.h"
#include "CFG.h"
#include <graphviz/cgraph.h>
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class BasicBlock;
class CFG;

class Function {
public:
  typedef ListDigraph::NodeMap<BasicBlock *> BasicBlockMap;
  typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;

  string name_;
  ListDigraph *graph_;
  ListDigraph::Node entry_;
  ListDigraph::Node exit_;
  BasicBlockMap *basicblocks_;
  
  
  Agraph_t *agraph_;
  AgnodeMap *agnodes_;
  
  Function (CFG &, string &);
  ListDigraph::Node addEntryPoint (BasicBlock &);
  ListDigraph::Node addBasicBlock (BasicBlock &);
  ListDigraph::Node addExitNode ();
  ListDigraph::Arc addControlFlowEdge (ListDigraph::Node, ListDigraph::Node);

  ListDigraph::Node findNode (string &);
};

#endif // _FUNCTION_H_
