#include "BasicBlock.h"

using namespace std;
using namespace lemon;

BasicBlock::BasicBlock (Function &f, string s)
{
  id_           = INVALID;
  label_        = s;
  name_         = f.label_ + "_" + label_;
  graph_        = new ListDigraph ();
  instructions_ = new InstructionMap (*graph_, NULL);
  leader_       = INVALID;
  call_         = NULL;
  return_       = INVALID;
}

ListDigraph::Node
BasicBlock::addInstruction (Instruction &i)
{
  ListDigraph::Node n = graph_ -> addNode ();
  (*instructions_)[n] = &i;
  i.id_ = n;
  
  return i.id_;
}

ListDigraph::Node
BasicBlock::addLeader (Instruction &i)
{
  leader_ = addInstruction (i);
  return leader_;
}

ListDigraph::Arc
BasicBlock::addFlowEdge (ListDigraph::Node n, ListDigraph::Node m)
{  
  ListDigraph::Arc a = graph_ -> addArc (n, m);
  return a;
}
