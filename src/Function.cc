#include "Function.h"

using namespace std;
using namespace lemon;

Function::Function (CFG &cfg, string &s)
{
  char *c_name  = (char *) ("cluster" + s).c_str ();
  char *c_label = (char *) (            s).c_str ();
  
  name_        = s;
  graph_       = new ListDigraph ();
  basicblocks_ = new BasicBlockMap (*graph_, NULL);
  entry_       = INVALID;
  exit_        = INVALID;

  agraph_      = agsubg (cfg.agraph_, c_name, TRUE);
  agnodes_     = new AgnodeMap (*graph_, NULL);
  
  agattr (agraph_, AGRAPH, (char *) "label", c_label);
  agattr (agraph_, AGRAPH, (char *) "style", (char *) "bold, rounded");
  agattr (agraph_, AGEDGE, (char *) "style", (char *) "solid");
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

ListDigraph::Node
Function::addBasicBlock (BasicBlock &bb)
{
  char *c_name  = (char *) (name_ + "_" + bb.name_).c_str ();
  char *c_label = (char *) (              bb.name_).c_str ();

  ListDigraph::Node n = graph_ -> addNode ();
  (*basicblocks_)[n] = &bb;

  (*agnodes_)[n] = agnode (agraph_, c_name, TRUE);
  agsafeset ((*agnodes_)[n], (char *) "label", c_label, (char *) "error");
  
  return n;
}

ListDigraph::Arc
Function::addControlFlowEdge (ListDigraph::Node n, ListDigraph::Node m)
{
  ListDigraph::Arc a = graph_ -> addArc (n, m);
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
