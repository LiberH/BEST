#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "Instruction.h"
#include "Function.h"
#include <graphviz/cgraph.h>
#include <lemon/list_graph.h>
#include <lemon/path.h>

using namespace std;
using namespace lemon;

class Instruction;
class Function;

class BasicBlock {
public:
  typedef ListDigraph::NodeMap<Instruction *> InstrMap;
  typedef SimplePath<ListDigraph> Path;
  typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;

  string name_;
  ListDigraph *graph_;
  InstrMap *instrs_;
  Path *path_;
  
  Agraph_t *agraph_;
  AgnodeMap *agnodes_;
  
  BasicBlock (Function &, string &);
  ListDigraph::Node addInstruction (Instruction &);
  ListDigraph::Arc addFlowEdge (ListDigraph::Node, ListDigraph::Node);
};

#endif // _BASICBLOCK_H_
