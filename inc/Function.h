#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "CFG.h"
#include "BasicBlock.h"
#include <lemon/list_graph.h>
#include <graphviz/cgraph.h>

using namespace std;
using namespace lemon;

class CFG;
class BasicBlock;
typedef ListDigraph::NodeMap<BasicBlock *> BasicBlockMap;
typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;

class Function {
public:
  string             name_;
  ListDigraph       *graph_;
  BasicBlockMap     *basicblocks_;
  ListDigraph::Node  entry_;
  ListDigraph::Node  exit_;
  
  Agraph_t          *agraph_;
  AgnodeMap         *agnodes_;
  
  
                    Function           (CFG &, string &);
  ListDigraph::Node addEntry           (BasicBlock &);
  ListDigraph::Node addExit            ();
  ListDigraph::Node addBasicBlock      (BasicBlock &);
  ListDigraph::Arc  addControlFlowEdge (ListDigraph::Node, ListDigraph::Node);
  
  ListDigraph::Node findNode           (string &);
};

#endif // _FUNCTION_H_
