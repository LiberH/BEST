#include "Function.h"
#include "CFG.h"
#include "BasicBlock.h"

using namespace std;
using namespace lemon;

Function::Function (CFG &cfg, string s)
{
  id_          = INVALID;
  label_       = s;
  name_        = "cluster" + label_;
  graph_       = new ListDigraph ();
  basicblocks_ = new BasicBlockMap (*graph_, NULL);
  entry_       = INVALID;
  exit_        = INVALID;
}

ListDigraph::Node
Function::addBasicBlock (BasicBlock &bb)
{
  ListDigraph::Node n = graph_ -> addNode ();
  (*basicblocks_)[n] = &bb;
  bb.id_ = n;
  
  return bb.id_;
}

ListDigraph::Node
Function::addEntry (BasicBlock &bb)
{
  entry_ = addBasicBlock (bb);
  return entry_;
}

ListDigraph::Node
Function::addExit ()
{
  string name = "exit";
  BasicBlock *bb = new BasicBlock (*this, name);
  exit_ = addBasicBlock (*bb);
  return exit_;
}

ListDigraph::Arc
Function::addControlFlowEdge (ListDigraph::Node n, ListDigraph::Node m)
{
  ListDigraph::Arc a = graph_ -> addArc (n, m);
  return a;
}


BasicBlock *
Function::findNode (string s)
{
  BasicBlock *bb;
  ListDigraph::NodeIt n (*graph_);
  for (; n != INVALID; ++n)
    {
      bb = (*basicblocks_)[n];
      if (bb -> label_ == s)
	break;
    }

  return bb;
}
