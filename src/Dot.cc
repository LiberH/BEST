#include "Dot.h"

#include "CFG.h"
#include "Function.h"
#include "BasicBlock.h"
#include "Instruction.h"

#include <iostream>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

static int putstr (void *, const char *);
static int flush (void *);
static Agiodisc_t agiodisc = { AgIoDisc.afread, &putstr, &flush }; 
static Agdisc_t agdisc = { NULL, NULL, &agiodisc };

AgnodeMap *
Dot::toDot_ (BasicBlock &bb, Agraph_t *agraph)
{
  Graph *graph = bb.m_graph;
  AgnodeMap *agnodes = new AgnodeMap (*graph);
  NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      Instruction *i = (*bb.m_instrs)[n];
      (*agnodes)[n] = agnode (agraph, C(i -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(i -> m_label), "error");
    }
  
  ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      Node n = graph -> source (a);
      Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  return agnodes;
}

string
Dot::toDot (BasicBlock &bb)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(bb.m_label), Agdirected, &agdisc);

  toDot_ (bb, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

AgnodeMap *
Dot::toDot_ (Function &f, Agraph_t *agraph, LabelMap *m)
{
  agattr (agraph, AGRAPH, "label", C(f.m_label));
  agattr (agraph, AGRAPH, "style", "bold, rounded");
  agattr (agraph, AGEDGE, "style", "solid");

  Graph *graph = f.m_graph;
  AgnodeMap *agnodes = new AgnodeMap (*graph);  
  NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BasicBlock *bb = (*f.m_bbs)[n];
      string label = m ? (*m)[n] : bb -> m_label;
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(label), "error");
    }
    
  ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      Node n = graph -> source (a);
      Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  return agnodes;
}

string
Dot::toDot (Function &f, LabelMap *m)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(f.m_label), Agdirected, &agdisc);

  toDot_ (f, agraph, m);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}
/*
string
Dot::toDot (Function &f, LabelMap &labels)
{
  AgnodeMap *agnodes;
  ostringstream oss;
  Agraph_t *agraph = agopen (C(f.label), Agdirected, &agdisc);

  agnodes = toDot (f, agraph);
  NodeIt n (*f.graph);
  for (; n != INVALID; ++n)
    agsafeset ((*agnodes)[n], "label", C(labels[n]), "error");
  
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}
*/
/* --- */

void
Dot::toDot_ (CFG &cfg, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(cfg.m_label));
  agattr (agraph, AGEDGE, "style", "bold");
  
  NodeIt n (*cfg.m_graph);
  for (; n != INVALID; ++n)
    {
      Function *f = (*cfg.m_functs)[n];
      Agraph_t *subg = agsubg (agraph, C("cluster" + f -> m_label), TRUE);
      AgnodeMap *agnodes = toDot_ (*f, subg);

      NodeIt m (*f -> m_graph);
      for (; m != INVALID; ++m)
	{
	  BasicBlock *bb = (*f -> m_bbs)[m];
	  if (bb -> m_call)
	    {
	      Function *g = bb -> m_call;
	      Node      o = bb -> m_ret;

	      // Dashize the "call-site to return-site" edge:
	      Agedge_t *age = agedge (agraph, (*agnodes)[m], (*agnodes)[o], NULL, FALSE);
	      agsafeset (age, "style", "dashed", "error");
	      
	      BasicBlock *bbe  = (*g -> m_bbs)[g -> m_entry];
	      BasicBlock *bbx  = (*g -> m_bbs)[g -> m_exit ];
	      Agraph_t   *subg = agsubg (agraph, C("cluster" + g -> m_label), FALSE);
	      Agnode_t   *agne = agnode (subg, C(bbe -> m_name), FALSE);
	      Agnode_t   *agnx = agnode (subg, C(bbx -> m_name), FALSE);
	      agedge (agraph, (*agnodes)[m], agne, NULL, TRUE);	      
	      agedge (agraph, agnx, (*agnodes)[o], NULL, TRUE);
	    }
	}
    }
}

string
Dot::toDot (CFG &cfg)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(cfg.m_label), Agdirected, &agdisc);

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
