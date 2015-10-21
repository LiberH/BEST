#include "Dot.h"
#include "PDT.h"
#include "DFSTree.h"
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
  
  CFG *rcfg = CFG::reverse (*cfg);
  f.open ("test/rcfg.dot");
  f << Dot::toDot (*rcfg);
  f.close ();

  DFSTree *dfsg = new DFSTree (*rcfg);
  f.open ("test/dfsg.dot");
  f << Dot::toDot (*dfsg);
  f.close ();

  DFSTree *dfst = DFSTree::tree (*dfsg);
  f.open ("test/dfst.dot");
  f << Dot::toDot (*dfst);
  f.close ();
  
  PDT *pdt = new PDT (*rcfg, *dfst);
  f.open ("test/pdt.dot");
  f << Dot::toDot (*pdt);
  f.close ();

  return EXIT_SUCCESS;
}

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
