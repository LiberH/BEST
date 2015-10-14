#include "Dot.h"
#include "ICFG.h"
#include "PDT.h"
#include "DFTree.h"
#include "CFG.h"
#include "BB.h"
#include "Inst.h"
#include <iostream>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

static int putstr (void *, const char *);
static int flush (void *);
static Agiodisc_t agiodisc = { AgIoDisc.afread, &putstr, &flush }; 
static Agdisc_t agdisc = { NULL, NULL, &agiodisc };

ListDigraph::NodeMap<Agnode_t *> *
Dot::toDot_ (BB &bb, Agraph_t *agraph)
{
  ListDigraph *graph = bb.m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      Inst *i = (*bb.m_instrs)[n];
      (*agnodes)[n] = agnode (agraph, C(i -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(i -> m_label), "error");
    }
  
  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  return agnodes;
}

string
Dot::toDot (BB &bb)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(bb.m_label), Agdirected, &agdisc);

  toDot_ (bb, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

ListDigraph::NodeMap<Agnode_t *> *
Dot::toDot_ (CFG &f, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(f.m_label));
  agattr (agraph, AGRAPH, "style", "bold, rounded");
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = f.m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);  
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*f.m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
    }

  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }
  
  return agnodes;
}

string
Dot::toDot (CFG &f)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(f.m_label), Agdirected, &agdisc);

  toDot_ (f, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

ListDigraph::NodeMap<Agnode_t *> *
Dot::toDot_ (DFTree &t, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(t.m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = t.m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);
  ListDigraph::NodeIt n (*t.m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*t.m_cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_label), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_name), "error");
    }

  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[(*t.m_ref)[n]], (*agnodes)[(*t.m_ref)[m]], NULL, TRUE);
    }
  
  return agnodes;
}

string
Dot::toDot (DFTree &t)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(t.m_label), Agdirected, &agdisc);

  toDot_ (t, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

ListDigraph::NodeMap<Agnode_t *> *
Dot::toDot_ (PDT &pdt, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(pdt.m_label));

  ListDigraph *graph = pdt.m_graph;
  //CFG *f = pdt.m_funct;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      //BB *bb = (*f -> m_bbs)[(*nr)[n]];
      (*agnodes)[n] = agnode (agraph, NULL, TRUE);//C(bb -> m_name), TRUE);
      //agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
    }
    
  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  return agnodes;
}

string
Dot::toDot (PDT &pdt)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(pdt.m_label), Agdirected, &agdisc);

  toDot_ (pdt, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

/* --- */

void
Dot::toDot_ (ICFG &cfg, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(cfg.m_label));
  agattr (agraph, AGEDGE, "style", "bold");
  
  ListDigraph::NodeIt n (*cfg.m_graph);
  for (; n != INVALID; ++n)
    {
      CFG *f = (*cfg.m_cfgs)[n];
      Agraph_t *subg = agsubg (agraph, C("cluster" + f -> m_label), TRUE);
      ListDigraph::NodeMap<Agnode_t *> *agnodes = toDot_ (*f, subg);

      ListDigraph::NodeIt m (*f -> m_graph);
      for (; m != INVALID; ++m)
	{
	  BB *bb = (*f -> m_bbs)[m];
	  if (bb -> m_call)
	    {
	      CFG *g = bb -> m_call;
	      ListDigraph::Node o = bb -> m_ret;

	      // Dashize the "call-site to return-site" edge:
	      Agedge_t *age = agedge (agraph, (*agnodes)[m], (*agnodes)[o], NULL, FALSE);
	      agsafeset (age, "style", "dashed", "error");
	      
	      BB *bbe  = (*g -> m_bbs)[g -> m_entry];
	      BB *bbx  = (*g -> m_bbs)[g -> m_exit ];
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
Dot::toDot (ICFG &cfg)
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
