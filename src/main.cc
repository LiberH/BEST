#include "Dot.h"
#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"
#include "CFG.h"
#include "DFS.h"

using namespace std;
using namespace lemon;

BasicBlock
*createBB (Function &f, string name)
{
  BasicBlock  *bb = new BasicBlock (f, name);
  Instruction *i0 = new Instruction (*bb, "I0");
  Instruction *i1 = new Instruction (*bb, "I1");
  
  ListDigraph::Node n0 = bb -> addLeader (*i0);
  ListDigraph::Node n1 = bb -> addInstruction (*i1);

  bb -> addFlowEdge (n0, n1);
  return bb;
}

Function
*createF0 (CFG &cfg)
{
  Function   *f   = new Function (cfg, "F0");
  BasicBlock *bb0 = createBB (*f, "BB0");
  BasicBlock *bb1 = createBB (*f, "BB1");
  BasicBlock *bb2 = createBB (*f, "BB2");
  BasicBlock *bb3 = createBB (*f, "BB3");
  
  ListDigraph::Node n0 = f -> addEntry (*bb0);
  ListDigraph::Node n1 = f -> addBasicBlock (*bb1);
  ListDigraph::Node n2 = f -> addBasicBlock (*bb2);
  ListDigraph::Node n3 = f -> addBasicBlock (*bb3);
  ListDigraph::Node nx = f -> addExit ();

  f -> addControlFlowEdge (n0, n1);
  f -> addControlFlowEdge (n0, n2);
  f -> addControlFlowEdge (n3, n2);
  f -> addControlFlowEdge (n2, nx);
  
  return f;
}

Function
*createF1 (CFG &cfg)
{
  Function   *f   = new Function (cfg, "F1");
  BasicBlock *bb0 = createBB (*f, "BB0");
  BasicBlock *bb1 = createBB (*f, "BB1");
  BasicBlock *bb2 = createBB (*f, "BB2");
  
  ListDigraph::Node n0 = f -> addEntry (*bb0);
  ListDigraph::Node n1 = f -> addBasicBlock (*bb1);
  ListDigraph::Node n2 = f -> addBasicBlock (*bb2);
  ListDigraph::Node nx = f -> addExit ();

  f -> addControlFlowEdge (n0, n1);
  f -> addControlFlowEdge (n1, n0);
  f -> addControlFlowEdge (n1, n2);
  f -> addControlFlowEdge (n2, nx);

  return f;
}

CFG
*createCFG ()
{
  CFG      *cfg = new CFG ("CFG");
  Function *f0  = createF0 (*cfg);
  Function *f1  = createF1 (*cfg);
  
  cfg -> addFunction (*f0);
  cfg -> addFunction (*f1);
  cfg -> addCall (*f0, "BB1", "BB3", *f1);
  
  return cfg;
}


template <typename T>
LabelMap
*toLabels (ListDigraph &g, ListDigraph::NodeMap<T> *map)
{
  LabelMap *labels = new LabelMap (g);
  ListDigraph::NodeIt n (g);
  for (; n != INVALID; ++n)
    {
      ostringstream oss;
      oss << (*map)[n];
      (*labels)[n] = oss.str ();
    }
    
  return labels;
}

int
main()
{
  CFG      *cfg = createCFG ();
  Function *f1  = cfg -> findNode ("F1");

  DistMap *dists = dfs (*f1);
  LabelMap *labels = toLabels (*f1 -> graph_, dists);
  cout << Dot::toDot (*f1, *labels) << endl;
  return 0;
}
