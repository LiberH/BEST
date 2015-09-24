#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "BasicBlock.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class BasicBlock;

class Instruction {
public:
  ListDigraph::Node id_;
  string            label_;
  string            name_;

  
  Instruction (BasicBlock &, string);
};

#endif // _INSTRUCTION_H_
