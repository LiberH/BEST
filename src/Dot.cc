#include "Dot.h"
#include <iostream>
#include <lemon/list_graph.h>
#include <graphviz/cgraph.h>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

static int putstr (void *, const char *);
static int flush (void *);
static Agiodisc_t agiodisc = { AgIoDisc.afread, &putstr, &flush }; 
static Agdisc_t agdisc = { NULL, NULL, &agiodisc };

typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;

static AgnodeMap
*toDot_ (BasicBlock &bb, Agraph_t *agraph)
{
  AgnodeMap *agnodes = new AgnodeMap (*bb.graph_);
  ListDigraph::NodeIt n (*bb.graph_);
  for (; n != INVALID; ++n)
    {
      Instruction *i = (*bb.instructions_)[n];
      (*agnodes)[n] = agnode (agraph, C(i -> name_), TRUE);
      agsafeset ((*agnodes)[n], "label", C(i -> label_), "error");
    }
  
  ListDigraph::ArcIt a (*bb.graph_);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = bb.graph_ -> source (a);
      ListDigraph::Node m = bb.graph_ -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  return agnodes;
}

string
Dot::toDot (BasicBlock &bb)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(bb.label_), Agdirected, &agdisc);

  toDot_ (bb, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

static AgnodeMap
*toDot_ (Function &f, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(f.label_));
  agattr (agraph, AGRAPH, "style", "bold, rounded");
  agattr (agraph, AGEDGE, "style", "solid");

  AgnodeMap *agnodes = new AgnodeMap (*f.graph_);  
  ListDigraph::NodeIt n (*f.graph_);
  for (; n != INVALID; ++n)
    {
      BasicBlock *bb = (*f.basicblocks_)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> name_), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> label_), "error");
      
      // Agraph_t *subg = agsubg (agraph, C(bb -> name_), TRUE);
      // toDot_ (*bb, subg);
    }
    
  ListDigraph::ArcIt a (*f.graph_);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = f.graph_ -> source (a);
      ListDigraph::Node m = f.graph_ -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  return agnodes;
}

string
Dot::toDot (Function &f)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(f.label_), Agdirected, &agdisc);

  toDot_ (f, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

string
Dot::toDot (Function &f, LabelMap &labels)
{
  AgnodeMap *agnodes;
  ostringstream oss;
  Agraph_t *agraph = agopen (C(f.label_), Agdirected, &agdisc);

  agnodes = toDot_ (f, agraph);
  ListDigraph::NodeIt n (*f.graph_);
  for (; n != INVALID; ++n)
    agsafeset ((*agnodes)[n], "label", C(labels[n]), "error");
  
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

static void
toDot_ (CFG &cfg, Agraph_t *agraph)
{
  AgnodeMap *agnodes;
  agattr (agraph, AGRAPH, "label", C(cfg.label_));
  agattr (agraph, AGEDGE, "style", "bold");
  
  ListDigraph::NodeIt n (*cfg.graph_);
  for (; n != INVALID; ++n)
    {
      Function *f = (*cfg.functions_)[n];
      Agraph_t *subg = agsubg (agraph, C(f -> name_), TRUE);
      agnodes = toDot_ (*f, subg);

      ListDigraph::NodeIt m (*f -> graph_);
      for (; m != INVALID; ++m)
	{
	  BasicBlock *bb = (*f -> basicblocks_)[m];
	  if (bb -> call_ != NULL)
	    {
	      ListDigraph::Node o = bb -> return_;
	      Agedge_t *age = agedge (agraph, (*agnodes)[m], (*agnodes)[o], NULL, TRUE);
	      agsafeset (age, "style", "dashed", "error");
	      
	      Function *g = bb -> call_;
	      Agraph_t *subg = agsubg (agraph, C(g -> name_), FALSE);
	      BasicBlock *bbe = (*g -> basicblocks_)[g -> entry_];
	      Agnode_t *agne = agnode (subg, C(bbe -> name_), FALSE);
	      agedge (agraph, (*agnodes)[m], agne, NULL, TRUE);	      
	      
	      BasicBlock *bbx = (*g -> basicblocks_)[g -> exit_];
	      Agnode_t *agnx = agnode (subg, C(bbx -> name_), FALSE);
	      agedge (agraph, agnx, (*agnodes)[o], NULL, TRUE);
	    }

	}
    }
}

string
Dot::toDot (CFG &cfg)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(cfg.label_), Agdirected, &agdisc);

  toDot_ (cfg, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
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
