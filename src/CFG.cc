#include "CFG.h"

using namespace std;
using namespace lemon;

extern Agdisc_t agdisc;

CFG::CFG (string &s)
{
  name_ = s;
  graph_ = new ListDigraph ();
  functions_ = new FunctionMap (*graph_);

  char *c_name = (char *) name_.c_str ();
  agraph_ = agopen (c_name, Agdirected, &agdisc);
  agattr (agraph_, AGRAPH, (char *) "label", c_name);
  agattr (agraph_, AGEDGE, (char *) "style", (char *) "bold");
  agnodes_ = new AgnodeMap (*graph_);
}

ListDigraph::Node
CFG::addFunction (Function &f)
{
  ListDigraph::Node n, m;
  n = graph_ -> addNode ();
  (*functions_)[n] = &f;

  /*
  ostringstream oss; oss << name_ << "_" << f.name_;
  char *c_name = (char *) oss.str().c_str ();
  (*agnodes_)[n] = agnode (agraph_, c_name, TRUE);
  */
  
  return n;
}

ListDigraph::Arc
CFG::addCall (string &s, string &t, Function &f, Function &g)
{
  ListDigraph::Arc a = INVALID;
  /* TODO:
   * - add call and return edge in the CFG
   *   (needs a arc structure with reference to each graph)
   */

  ListDigraph::Node n = f.findNode (s);
  ListDigraph::Node m = f.findNode (t);
  ListDigraph::Node e = g.entry_;
  ListDigraph::Node x = g.exit_;
  
  agedge (agraph_, (*f.agnodes_)[n], (*g.agnodes_)[e], (char*)"here", TRUE);    
  agedge (agraph_, (*g.agnodes_)[x], (*f.agnodes_)[m], (char*)"there", TRUE);

  Agedge_t *age = agedge (f.agraph_, (*f.agnodes_)[n], (*f.agnodes_)[m], (char*)"here", TRUE);    
  agsafeset (age, (char *) "style", (char *) "dashed", (char *) "error");
  
  return a;
}
