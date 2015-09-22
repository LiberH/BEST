#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "Function.h"
#include "Instruction.h"
#include <lemon/list_graph.h>
#include <graphviz/cgraph.h>

using namespace std;
using namespace lemon;

class Function;
class Instruction;
typedef ListDigraph::NodeMap<Instruction *> InstructionMap;

class BasicBlock {
public:
  string             name_;
  ListDigraph       *graph_;
  InstructionMap    *instructions_;
  ListDigraph::Node  leader_;
  Function          *call_;
  BasicBlock        *return_;

  
                    BasicBlock         (Function &, string &);
  ListDigraph::Node addLeader          (Instruction &);
  ListDigraph::Node addInstruction     (Instruction &);
  ListDigraph::Arc  addFallThroughEdge (ListDigraph::Node, ListDigraph::Node);
};

#endif // _BASICBLOCK_H_
