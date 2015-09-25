#include "CFG.h"
#include "Function.h"
#include "BasicBlock.h"
#include <iostream>

using namespace std;
using namespace lemon;

CFG::CFG (string s)
{
  label_     = s;
  graph_     = new ListDigraph ();
  functions_ = new FunctionMap (*graph_, NULL);
}

ListDigraph::Node
CFG::addFunction (Function &f)
{
  ListDigraph::Node n = graph_ -> addNode ();
  (*functions_)[n] = &f;
  f.id_ = n;
  
  return f.id_;
}

void
CFG::addCall (Function &f, string s, string t, Function &g)
{
  BasicBlock *cs = f.findNode (s);
  BasicBlock *rs = f.findNode (t);
  cs -> return_ = rs -> id_;
  cs -> call_ = &g;
}


Function *
CFG::findNode (string s)
{
  Function *f;
  ListDigraph::NodeIt n (*graph_);
  for (; n != INVALID; ++n)
    {
      f = (*functions_)[n];
      if (f -> label_ == s)
	break;
    }

  return f;
}
