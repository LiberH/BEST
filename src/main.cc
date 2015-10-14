#include "Dot.h"
#include "PDT.h"
#include "DFTree.h"
#include "CFG.h"
#include "BB.h"
#include "Inst.h"
#include <stdlib.h>
#include <fstream>

using namespace lemon;
using namespace std;

static CFG  *createCFG ();
static void  createBB (BB &);

int
main (int argc, char *argv[])
{
  ofstream f;
  CFG *cfg = createCFG ();
  f.open ("test/cfg.dot");
  f << Dot::toDot (*cfg);
  f.close ();
  
  CFG *reverse_cfg = CFG::reverse (*cfg);
  f.open ("test/rcfg.dot");
  f << Dot::toDot (*reverse_cfg);
  f.close ();

  DFTree *dfs_tree = new DFTree (*reverse_cfg);
  f.open ("test/dfs.dot");
  f << Dot::toDot (*dfs_tree);
  f.close ();
  
  PDT *pdt = new PDT (*reverse_cfg, *dfs_tree);
  f.open ("test/pdt.dot");
  f << Dot::toDot (*pdt);
  f.close ();

  return EXIT_SUCCESS;
}

/*
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
*/

static CFG *
createCFG ()
{
  CFG *f = new CFG;
  
  BB *bb0 = new BB;
  BB *bb1 = new BB;
  BB *bb2 = new BB;
  BB *bb3 = new BB;
  BB *bb4 = new BB;
  BB *bb5 = new BB;
  BB *bb6 = new BB;
  BB *bb7 = new BB;

  createBB (*bb0);
  createBB (*bb1);
  createBB (*bb2);
  createBB (*bb3);
  createBB (*bb4);
  createBB (*bb5);
  createBB (*bb6);
  createBB (*bb7);
  
  ListDigraph::Node n0 = f -> addNode (*bb0);
  ListDigraph::Node n1 = f -> addNode (*bb1);
  ListDigraph::Node n2 = f -> addNode (*bb2);
  ListDigraph::Node n3 = f -> addNode (*bb3);
  ListDigraph::Node n4 = f -> addNode (*bb4);
  ListDigraph::Node n5 = f -> addNode (*bb5);
  ListDigraph::Node n6 = f -> addNode (*bb6);
  ListDigraph::Node n7 = f -> addNode (*bb7);

  f -> entry (n5);
  f -> exit  (n0);
  
  f -> addEdge (n5, n4);
  f -> addEdge (n5, n7);
  f -> addEdge (n4, n3);
  f -> addEdge (n3, n2);
  f -> addEdge (n3, n1);
  f -> addEdge (n2, n1);  
  f -> addEdge (n1, n0);  
  f -> addEdge (n7, n6);
  f -> addEdge (n6, n2);

  return f;
}

static void
createBB (BB &bb)
{
  Inst *i0 = new Inst;
  Inst *i1 = new Inst;
  
  ListDigraph::Node n0 = bb.addNode (*i0);
  ListDigraph::Node n1 = bb.addNode (*i1);
  
  bb.leader (n0);
  bb.addEdge (n0, n1);
}
