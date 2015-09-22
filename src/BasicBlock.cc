#include "BasicBlock.h"

using namespace std;
using namespace lemon;

BasicBlock::BasicBlock (Function &fn, string &s)
{
  name_         = s;
  graph_        = new ListDigraph ();
  instructions_ = new InstructionMap (*graph_, NULL);
  leader_       = INVALID;
  call_         = NULL;
  return_       = NULL;
}

ListDigraph::Node
BasicBlock::addLeader (Instruction &i)
{
  leader_ = addInstruction (i);
  return leader_;
}

ListDigraph::Node
BasicBlock::addInstruction (Instruction &i)
{
  ListDigraph::Node n = graph_ -> addNode ();
  (*instructions_)[n] = &i;  
  return n;
}

ListDigraph::Arc
BasicBlock::addFallThroughEdge (ListDigraph::Node n, ListDigraph::Node m)
{  
  ListDigraph::Arc a = graph_ -> addArc (n, m);
  return a;
}
