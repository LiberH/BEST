#include "Function.h"

using namespace std;
using namespace lemon;

Function::Function (CFG &cfg, string &s)
{
  name_ = s;
  graph_ = new ListDigraph ();
  basicblocks_ = new BasicBlockMap (*graph_);

  char *c_name = (char *) ("cluster" + name_).c_str ();
  agraph_ = agsubg (cfg.agraph_, c_name, TRUE);

  c_name = (char *) name_.c_str ();
  agattr (agraph_, AGRAPH, (char *) "label", c_name);
  agattr (agraph_, AGRAPH, (char *) "style", (char *) "bold,rounded");
  agattr (agraph_, AGEDGE, (char *) "style", (char *) "solid");
  agnodes_ = new AgnodeMap (*graph_);
}

ListDigraph::Node
Function::addEntryPoint (BasicBlock &bb)
{
  ListDigraph::Node n;  
  n = addBasicBlock (bb);
  entry_ = n;
  
  return n;
}


ListDigraph::Node
Function::addBasicBlock (BasicBlock &bb)
{
  ListDigraph::Node n, m;
  n = graph_ -> addNode ();
  (*basicblocks_)[n] = &bb;

  ostringstream oss; oss << name_ << "_" << bb.name_;
  char *c_name = (char *) oss.str().c_str ();
  (*agnodes_)[n] = agnode (agraph_, c_name, TRUE);
  
  c_name = (char *) bb.name_.c_str ();
  agsafeset ((*agnodes_)[n], (char *) "label", c_name, (char *) "error");
  
  return n;
}

ListDigraph::Node
Function::addExitNode ()
{
  string name = "exit";
  BasicBlock *bb = new BasicBlock (*this, name);
  
  ListDigraph::Node n, m;
  n = graph_ -> addNode ();
  (*basicblocks_)[n] = bb;
  exit_ = n;

  ostringstream oss; oss << name_ << "_" << bb -> name_;
  char *c_name = (char *) oss.str().c_str ();
  (*agnodes_)[n] = agnode (agraph_, c_name, TRUE);

  c_name = (char *) bb -> name_.c_str ();
  agsafeset ((*agnodes_)[n], (char *) "label", c_name, (char *) "error");

  return n;
}

ListDigraph::Arc
Function::addControlFlowEdge (ListDigraph::Node n, ListDigraph::Node m)
{
  ListDigraph::Arc a;
  a = graph_ -> addArc (n, m);

  agedge (agraph_, (*agnodes_)[n], (*agnodes_)[m], NULL, TRUE);  
  return a;
}

ListDigraph::Node
Function::findNode (string &s)
{
  BasicBlock *bb;
  ListDigraph::NodeIt n (*graph_);
  for (; n != INVALID; ++n)
    {
      bb = (*basicblocks_)[n];
      if (bb -> name_ == s)
	  break;
    }
  
  return n;
}
