#include "Dot.hpp"
#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "ICFG.hpp"

#include <lemon/list_graph.h>
#include <stdio.h>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

void
Dot::toFile (string path, BB &bb)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(bb.m_label), Agdirected, NULL);
  renderBB (agraph, bb);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::renderBB (Agraph_t *agraph, BB &bb)
{
  agattr (agraph, AGRAPH, "label", C(bb.m_label));  
  agattr (agraph, AGEDGE, "style", "solid");

  Agnode_t *agn;
  vector<Agnode_t *> *agnodes = new vector<Agnode_t *>;
  vector<Inst *>::iterator it = bb.m_instrs -> begin();
  
  agn = agnode (agraph, C((*it) -> m_name), TRUE);
  agsafeset (agn, "label", C((*it) -> m_label), "error");
  agnodes -> push_back (agn);
  
  for (++it; it != bb.m_instrs -> end(); ++it)
    {
      agn = agnode (agraph, C((*it) -> m_name), TRUE);
      agedge (agraph, agnodes -> back (), agn, NULL, TRUE);
      agsafeset (agn, "label", C((*it) -> m_label), "error");
      agnodes -> push_back (agn);
    }
}

void
Dot::toFile (string path, CFG &cfg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(cfg.m_label), Agdirected, NULL);
  renderCFG (agraph, cfg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

ListDigraph::NodeMap<Agnode_t *> *
Dot::renderCFG (Agraph_t *agraph, CFG &cfg)
{
  agattr (agraph, AGRAPH, "label", C(cfg.m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg.m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);  
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg.m_bbs)[n];
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

void
Dot::toFile (string path, ICFG &icfg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(icfg.m_label), Agdirected, NULL);
  renderICFG (agraph, icfg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::renderICFG (Agraph_t *agraph, ICFG &icfg)
{
  agattr (agraph, AGRAPH, "label", C(icfg.m_label));
  agattr (agraph, AGEDGE, "style", "bold");
  
  ListDigraph *graph = icfg.m_graph;
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      CFG *cfg = (*icfg.m_cfgs)[n];
      Agraph_t *subg = agsubg (agraph, C("cluster" + cfg -> m_label), TRUE);
      ListDigraph::NodeMap<Agnode_t *> *agnodes = renderCFG (subg, *cfg);

      ListDigraph::NodeIt m (*cfg -> m_graph);
      for (; m != INVALID; ++m)
	{
	  BB *bb = (*cfg -> m_bbs)[m];
	  if (bb -> m_call)
	    {
	      // Dashize the "call-site to return-site" edge:
	      ListDigraph::Node o = bb -> m_ret;
	      Agedge_t *age = agedge (agraph, (*agnodes)[m], (*agnodes)[o], NULL, FALSE);
	      agsafeset (age, "style", "dashed", "solid");

	      CFG *trg = bb -> m_call;
	      Agraph_t *subg = agsubg (agraph, C("cluster" + trg -> m_label), FALSE);
	      BB *bbe = (*trg -> m_bbs)[trg -> m_entry];
	      Agnode_t *agne = agnode (subg, C(bbe -> m_name), FALSE);
	      agedge (agraph, (*agnodes)[m], agne, NULL, TRUE);
	      
	      BB *bbx = (*trg -> m_bbs)[trg -> m_exit ];
	      Agnode_t *agnx = agnode (subg, C(bbx -> m_name), FALSE);
	      agedge (agraph, agnx, (*agnodes)[o], NULL, TRUE);
	    }
	}
    }
}

/*
ListDigraph::NodeMap<Agnode_t *> *
Dot::toDot_ (DFSTree &t, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(t.m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = t.m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);
  ListDigraph::NodeIt n (*t.m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*t.m_cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");

      ostringstream oss;
      oss << "(" << (*t.m_order)[n] << ")";
      agsafeset ((*agnodes)[n], "xlabel", C(oss.str()), "error");
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
Dot::toDot (DFSTree &t)
{
  ostringstream oss;
  Agraph_t *agraph = agopen (C(t.m_label), Agdirected, &agdisc);

  toDot_ (t, agraph);
  agwrite (agraph, &oss);
  agclose (agraph);
  return oss.str ();
}

// ---

ListDigraph::NodeMap<Agnode_t *> *
Dot::toDot_ (PDT &pdt, Agraph_t *agraph)
{
  agattr (agraph, AGRAPH, "label", C(pdt.m_label));

  ListDigraph *graph = pdt.m_graph;
  CFG *cfg = pdt.m_cfg;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
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
*/
