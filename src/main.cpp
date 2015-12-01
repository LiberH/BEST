#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "ICFG.hpp"
#include "Dot.hpp"

#include <stdlib.h>
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

int
main (int argc, char *argv[])
{
  Inst *cfg0bb0i0 = new Inst ("I0");
  Inst *cfg0bb0i1 = new Inst ("I1");
  BB   *cfg0bb0   = new BB ("BB0");
  cfg0bb0 -> addNode (*cfg0bb0i0);
  cfg0bb0 -> addNode (*cfg0bb0i1);

  Inst *cfg0bb1i0 = new Inst ("I0");
  Inst *cfg0bb1i1 = new Inst ("I1");
  BB   *cfg0bb1   = new BB ("BB1");
  cfg0bb1 -> addNode (*cfg0bb1i0);
  cfg0bb1 -> addNode (*cfg0bb1i1);

  Inst *cfg0bb2i0 = new Inst ("I0");
  Inst *cfg0bb2i1 = new Inst ("I1");
  BB   *cfg0bb2   = new BB ("BB2");
  cfg0bb2 -> addNode (*cfg0bb2i0);
  cfg0bb2 -> addNode (*cfg0bb2i1);
  
  CFG *cfg0 = new CFG ("CFG0");
  ListDigraph::Node ncfg0bb0 = cfg0 -> addNode (*cfg0bb0);
  ListDigraph::Node ncfg0bb1 = cfg0 -> addNode (*cfg0bb1);
  ListDigraph::Node ncfg0bb2 = cfg0 -> addNode (*cfg0bb2);
  cfg0 -> addEdge (*cfg0bb0, *cfg0bb1);
  cfg0 -> addEdge (*cfg0bb1, *cfg0bb2);
  cfg0 -> entry (ncfg0bb0);
  cfg0 -> exit  (ncfg0bb2);
  
  Inst *cfg1bb0i0 = new Inst ("I0");
  Inst *cfg1bb0i1 = new Inst ("I1");
  BB   *cfg1bb0   = new BB ("BB0");
  cfg1bb0 -> addNode (*cfg1bb0i0);
  cfg1bb0 -> addNode (*cfg1bb0i1);
  
  Inst *cfg1bb1i0 = new Inst ("I0");
  Inst *cfg1bb1i1 = new Inst ("I1");
  BB   *cfg1bb1   = new BB ("BB1");
  cfg1bb1 -> addNode (*cfg1bb1i0);
  cfg1bb1 -> addNode (*cfg1bb1i1);
  
  Inst *cfg1bb2i0 = new Inst ("I0");
  Inst *cfg1bb2i1 = new Inst ("I1");
  BB   *cfg1bb2   = new BB ("BB2");
  cfg1bb2 -> addNode (*cfg1bb2i0);
  cfg1bb2 -> addNode (*cfg1bb2i1);

  CFG *cfg1 = new CFG ("CFG1");
  ListDigraph::Node ncfg1bb0 = cfg1 -> addNode (*cfg1bb0);
  ListDigraph::Node ncfg1bb1 = cfg1 -> addNode (*cfg1bb1);
  ListDigraph::Node ncfg1bb2 = cfg1 -> addNode (*cfg1bb2);
  cfg1 -> addEdge (*cfg1bb0, *cfg1bb1);
  cfg1 -> addEdge (*cfg1bb1, *cfg1bb2);
  cfg1 -> entry (ncfg1bb0);
  cfg1 -> exit  (ncfg1bb2);

  ICFG *icfg = new ICFG ("ICFG");
  icfg -> addNode (*cfg0);
  icfg -> addNode (*cfg1);  
  cfg0bb1 -> call (*cfg1);
  cfg0bb1 -> ret  (ncfg0bb2);
  
  Dot::toFile ("test/bb0.dot", *cfg0bb0);
  Dot::toFile ("test/cfg0.dot", *cfg0);
  Dot::toFile ("test/icfg.dot", *icfg);
  
  return EXIT_SUCCESS;
}
