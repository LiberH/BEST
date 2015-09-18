#include "BasicBlock.h"

using namespace std;
using namespace lemon;

BasicBlock::BasicBlock (Function &fn, string &s)
{
  name_ = s;
  graph_ = new ListDigraph ();
  instrs_ = new InstrMap (*graph_);
  path_ = new Path ();

  /*
  char *c_name = (char *) ("cluster" + name_).c_str ();
  agraph_ = agsubg (fn.agraph_, c_name, TRUE);
  agattr (agraph_, AGEDGE, (char *) "style", (char *) "solid");
  agnodes_ = new AgnodeMap (*graph_);
  */
}

ListDigraph::Node
BasicBlock::addInstruction (Instruction &i)
{
  ListDigraph::Node n;
  n = graph_ -> addNode ();
  (*instrs_)[n] = &i;

  /*
  ostringstream oss; oss << name_ << "_" << i.name_;
  char *c_name = (char *) oss.str().c_str ();
  (*agnodes_)[n] = agnode (agraph_, c_name, TRUE);
  */
  
  return n;
}

ListDigraph::Arc
BasicBlock::addFlowEdge (ListDigraph::Node n, ListDigraph::Node m)
{  
  ListDigraph::Arc a;
  a = graph_ -> addArc (n, m);
  path_ -> addBack (a);

  /*
  ostringstream oss; oss << (*instrs_)[n] -> name_ << "_" << (*instrs_)[m] -> name_;
  char *c_name = (char *) oss.str().c_str ();
  agedge (agraph_, (*agnodes_)[n], (*agnodes_)[m], c_name, TRUE);
  */
  
  return a;
}
