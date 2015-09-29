#include "TestCase.h"
#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"
#include "CFG.h"
#include "DFS.h"
#include "Dot.h"

using namespace std;
using namespace lemon;

static void createCFG (CFG &);
static void createF0  (Function &, Function &);
static void createF1  (Function &);
static void createBB  (BasicBlock &);

TestCase::TestCase ()
{
  string cfg_name = "CFG";
  CFG *cfg = new CFG (cfg_name);
  createCFG (*cfg);

  string f1_name = "F1";
  Function *f1 = cfg -> findNode (f1_name);
  LabelMap *labels = DFS::labels (*f1);
  
  cout << Dot::toDot (*f1) << endl;
  cout << Dot::toDot (*f1, labels) << endl;
}

/* --- */

static void
createCFG (CFG &cfg)
{
  Function *f0 = new Function;
  Function *f1 = new Function;
  
  cfg.addNode (*f0);
  cfg.addNode (*f1);

  createF1 (*f1);
  createF0 (*f0, *f1);
}

static void
createF0 (Function &f0, Function &f1)
{
  BasicBlock *bb0 = new BasicBlock;
  BasicBlock *bb1 = new BasicBlock;
  BasicBlock *bb2 = new BasicBlock;
  BasicBlock *bb3 = new BasicBlock;
  BasicBlock *bb4 = new BasicBlock;
  
  createBB (*bb0);
  createBB (*bb1);
  createBB (*bb2);
  createBB (*bb3);
  createBB (*bb4);

  Node n0 = f0.addNode (*bb0);
  Node n1 = f0.addNode (*bb1);
  Node n2 = f0.addNode (*bb2);
  Node n3 = f0.addNode (*bb3);
  Node n4 = f0.addNode (*bb4);

  bb1 -> call (&f1);
  bb1 -> ret  (n3);
  
  f0.entry  (n0);
  f0.exit   (n4);
  
  f0.addEdge (n0, n1);
  f0.addEdge (n0, n2);
  f0.addEdge (n1, n3);
  f0.addEdge (n2, n4);
  f0.addEdge (n3, n2);
}

static void
createF1 (Function &f1)
{
  BasicBlock *bb0 = new BasicBlock;
  BasicBlock *bb1 = new BasicBlock;
  BasicBlock *bb2 = new BasicBlock;
  BasicBlock *bb3 = new BasicBlock;

  createBB (*bb0);
  createBB (*bb1);
  createBB (*bb2);
  createBB (*bb3);
  
  Node n0 = f1.addNode (*bb0);
  Node n1 = f1.addNode (*bb1);
  Node n2 = f1.addNode (*bb2);
  Node n3 = f1.addNode (*bb3);

  f1.entry (n0);
  f1.exit  (n3);
  
  f1.addEdge (n0, n1);
  f1.addEdge (n1, n0);
  f1.addEdge (n1, n2);
  f1.addEdge (n2, n3);
}

static void
createBB (BasicBlock &bb)
{
  Instruction *i0 = new Instruction;
  Instruction *i1 = new Instruction;
  
  Node n0 = bb.addNode (*i0);
  Node n1 = bb.addNode (*i1);
  
  bb.leader (n0);
  bb.addEdge (n0, n1);
}
