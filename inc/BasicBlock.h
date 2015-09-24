#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "Function.h"
#include "Instruction.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Function;
class Instruction;
typedef ListDigraph::NodeMap<Instruction *> InstructionMap;

class BasicBlock {
public:
  ListDigraph::Node  id_;
  string             label_;
  string             name_;
  ListDigraph       *graph_;
  InstructionMap    *instructions_;
  ListDigraph::Node  leader_;
  Function          *call_;
  ListDigraph::Node  return_;

  
                    BasicBlock     (Function &, string);
  ListDigraph::Node addInstruction (Instruction &);
  ListDigraph::Node addLeader      (Instruction &);
  ListDigraph::Arc  addFlowEdge    (ListDigraph::Node, ListDigraph::Node);
};

#endif // _BASICBLOCK_H_
