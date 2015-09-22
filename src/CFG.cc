#include "CFG.h"
#include <iostream>

using namespace std;
using namespace lemon;

static int putstr (void *, const char *);
static int flush (void *);
static Agiodisc_t agiodisc = { AgIoDisc.afread, &putstr, &flush }; 
static Agdisc_t agdisc = { NULL, NULL, &agiodisc };

CFG::CFG (string &s)
{
  char *c_name = (char *) s.c_str ();

  name_      = s;
  graph_     = new ListDigraph ();
  functions_ = new FunctionMap (*graph_, NULL);

  agraph_    = agopen (c_name, Agdirected, &agdisc);
  agnodes_   = new AgnodeMap (*graph_, NULL);
  
  agattr (agraph_, AGRAPH, (char *) "label", c_name);
  agattr (agraph_, AGEDGE, (char *) "style", (char *) "bold");
}

ListDigraph::Node
CFG::addFunction (Function &f)
{
  ListDigraph::Node n = graph_ -> addNode ();
  (*functions_)[n] = &f;
  return n;
}

void
CFG::addCall (Function &f, string &s, string &t, Function &g)
{
  ListDigraph::Node n = f.findNode (s);
  ListDigraph::Node m = f.findNode (t);
  BasicBlock *cs = (*f.basicblocks_)[n];
  BasicBlock *rs = (*f.basicblocks_)[m];
  cs -> call_ = &g;
  cs -> return_ = rs;

  ListDigraph::Node e = g.entry_;
  ListDigraph::Node x = g.exit_;
  Agedge_t *age = agedge (f.agraph_, (*f.agnodes_)[n], (*f.agnodes_)[m], NULL, TRUE);    
  agsafeset (age, (char *) "style", (char *) "dashed", (char *) "error");
  agedge (agraph_, (*f.agnodes_)[n], (*g.agnodes_)[e], NULL, TRUE);    
  agedge (agraph_, (*g.agnodes_)[x], (*f.agnodes_)[m], NULL, TRUE);
}


string
CFG::toDot ()
{
  ostringstream oss;
  agwrite (agraph_, &oss);
  return oss.str ();
}

/* Utils: */

static int
putstr (void *chan, const char *str)
{
  (*((ostream *) chan)) << str;
  return 0;
}

static int
flush (void *chan)
{
  (*((ostream *) chan)).flush ();
  return 0;
}
