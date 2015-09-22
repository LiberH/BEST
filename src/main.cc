#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"
#include "CFG.h"

#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace lemon;

Instruction
*createI (string name)
{  
  Instruction *i = new Instruction (name);
  return i;
}

BasicBlock
*createBB (Function &fn, string name)
{
  BasicBlock *bb = new BasicBlock (fn, name);

  Instruction *i0 = createI ("i0");
  ListDigraph::Node n0 = bb -> addLeader (*i0);
  
  return bb;
}

Function
*createF0 (CFG &cfg)
{
  string name = "F0";
  Function *fn = new Function (cfg, name);

  BasicBlock *bb0 = createBB (*fn, "BB0");
  BasicBlock *bb1 = createBB (*fn, "BB1");
  BasicBlock *bb2 = createBB (*fn, "BB2");
  BasicBlock *bb3 = createBB (*fn, "BB3");
  
  ListDigraph::Node n0 = fn -> addEntry (*bb0);
  ListDigraph::Node n1 = fn -> addBasicBlock (*bb1);
  ListDigraph::Node n2 = fn -> addBasicBlock (*bb2);
  ListDigraph::Node n3 = fn -> addBasicBlock (*bb3);
  ListDigraph::Node nx = fn -> addExit ();

  fn -> addControlFlowEdge (n0, n1);
  fn -> addControlFlowEdge (n0, n2);
  fn -> addControlFlowEdge (n3, n2);
  fn -> addControlFlowEdge (n2, nx);
  
  return fn;
}

Function
*createF1 (CFG &cfg)
{
  string name = "F1";
  Function *fn = new Function (cfg, name);

  BasicBlock *bb0  = createBB (*fn, "BB0");
  BasicBlock *bb1  = createBB (*fn, "BB1");
  BasicBlock *bb2  = createBB (*fn, "BB2");
  
  ListDigraph::Node n0 = fn -> addEntry (*bb0);
  ListDigraph::Node n1 = fn -> addBasicBlock (*bb1);
  ListDigraph::Node n2 = fn -> addBasicBlock (*bb2);
  ListDigraph::Node nx = fn -> addExit ();

  fn -> addControlFlowEdge (n0, n1);
  fn -> addControlFlowEdge (n1, n0);
  fn -> addControlFlowEdge (n1, n2);
  fn -> addControlFlowEdge (n2, nx);
  
  return fn;
}

CFG
*createCFG ()
{
  string name = "CFG";
  CFG* cfg = new CFG (name);

  Function *f0 = createF0 (*cfg);
  Function *f1 = createF1 (*cfg);
  cfg -> addFunction (*f0);
  cfg -> addFunction (*f1);

  string bb1 = "BB1";
  string bb3 = "BB3";
  cfg -> addCall (*f0, bb1, bb3, *f1);
  
  return cfg;
}

int
main()
{
  CFG *cfg = createCFG ();
  cout << cfg -> toDot () << endl;  
  return 0;
}
