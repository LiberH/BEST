#include "TestCase.h"
#include "staticInfo.h"

using namespace std;
using namespace lemon;

static CFG        *createCFG ();
static Function   *createF0  (CFG &);
static Function   *createF1  (CFG &);
static BasicBlock *createBB  (Function &, string);

TestCase::TestCase ()
{
  CFG      *cfg = createCFG ();
  Function *f1  = cfg -> findNode ("F1");

  DistMap *dists = dfs (*f1);
  LabelMap *labels = Dot::toLabels (*f1 -> graph_, dists);
  cout << Dot::toDot (*f1, *labels) << endl;
}

/* --- */

static CFG
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

static Function
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

static Function
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

static BasicBlock
*createBB (Function &f, string name)
{
  BasicBlock  *bb = new BasicBlock (f, name);
  staticInfo  *s0 = new staticInfo;
  staticInfo  *s1 = new staticInfo;
  Instruction *i0 = new Instruction (*bb, *s0);
  Instruction *i1 = new Instruction (*bb, *s1);
  
  ListDigraph::Node n0 = bb -> addLeader (*i0);
  ListDigraph::Node n1 = bb -> addInstruction (*i1);

  bb -> addFlowEdge (n0, n1);
  return bb;
}
