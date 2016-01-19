#include "Dot.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "ICFG.hpp"
#include "DFS.hpp"
#include "DT.hpp"
#include "CDG.hpp"
#include "DDG.hpp"
#include "PDG.hpp"

#include <stdio.h>
#include <lemon/list_graph.h>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

void
Dot::toFile (string path, BB &bb)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(bb.m_label), Agdirected, NULL);
  render (agraph, bb);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, BB &bb)
{
  agattr (agraph, AGRAPH, "label",   C(bb.m_label));  
  agattr (agraph, AGRAPH, "ranksep", "0.2");  
  agattr (agraph, AGNODE, "shape",   "box");  
  agattr (agraph, AGNODE, "width",   "2.0");  
  agattr (agraph, AGNODE, "height",  "0.3");  
  agattr (agraph, AGEDGE, "style",   "solid");

  Agnode_t *agn;
  vector<Agnode_t *> *agnodes = new vector<Agnode_t *>;
  vector<Inst *>::iterator it = bb.m_insts -> begin();
  
  agn = agnode (agraph, C((*it) -> m_name), TRUE);
  agsafeset (agn, "label", C((*it) -> m_label + "\\l"), "error");
  agnodes -> push_back (agn);
  
  for (++it; it != bb.m_insts -> end(); ++it)
    {
      agn = agnode (agraph, C((*it) -> m_name), TRUE);
      agedge (agraph, agnodes -> back (), agn, NULL, TRUE);
      agsafeset (agn, "label", C((*it) -> m_label + "\\l"), "error");
      agnodes -> push_back (agn);
    }
}

void
Dot::toFile (string path, CFG &cfg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(cfg.m_label), Agdirected, NULL);
  render (agraph, cfg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

//ListDigraph::NodeMap<Agnode_t *> *
void
Dot::render (Agraph_t *agraph, CFG &cfg)
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
  
  //return agnodes;
}
/*
void
Dot::toFile (string path, ICFG &icfg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(icfg.m_label), Agdirected, NULL);
  render (agraph, icfg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, ICFG &icfg)
{
  agattr (agraph, AGRAPH, "label", C(icfg.m_label));
  agattr (agraph, AGEDGE, "style", "bold");
  
  ListDigraph *graph = icfg.m_graph;
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      CFG *cfg = (*icfg.m_cfgs)[n];
      Agraph_t *subg = agsubg (agraph, C("cluster" + cfg -> m_label), TRUE);
      ListDigraph::NodeMap<Agnode_t *> *agnodes = render (subg, *cfg);

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
*/
void
Dot::toFile (string path, CFG &cfg, DFS &dfs)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(cfg.m_label), Agdirected, NULL);
  render (agraph, cfg, dfs);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, CFG &cfg, DFS &dfs)
{
  agattr (agraph, AGRAPH, "label", C(dfs.m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg.m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);  
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg.m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      ostringstream oss;
      oss << (*dfs.m_order)[n];
      agsafeset ((*agnodes)[n], "label", C(oss.str ()), "error");
    }
  
  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }
}

void
Dot::toFile (string path, DT &dt)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(dt.m_label), Agdirected, NULL);
  render (agraph, dt);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, DT &dt)
{
  agattr (agraph, AGRAPH, "label", C(dt.m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  const CFG *cfg = dt.m_cfg;
  ListDigraph *graph = cfg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);  
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
    }
  
  ListDigraph::NodeIt m (*graph);
  for (; m != INVALID; ++m)
    {
      ListDigraph::Node n = (*dt.m_idom)[m];
      if (n == INVALID) continue;
      agedge (agraph, (*agnodes)[(*dt.m_idom)[m]], (*agnodes)[m], NULL, TRUE);
    }
}

void
Dot::toFile (string path, CDG &cdg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(cdg.m_label), Agdirected, NULL);
  render (agraph, cdg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, CDG &cdg)
{
  agattr (agraph, AGRAPH, "label", C(cdg.m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  const CFG *cfg = cdg.m_cfg;
  ListDigraph *graph = cfg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);    
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
    }
  
  ListDigraph::NodeIt m (*cfg -> m_graph);
  for (; m != INVALID; ++m)
    {
      vector<ListDigraph::Node>::iterator it = (*cdg.m_deps)[m] -> begin ();
      for (; it != (*cdg.m_deps)[m] -> end (); ++it)
	agedge (agraph, (*agnodes)[(*it)], (*agnodes)[m], NULL, TRUE);
    }
}

void
Dot::toFile (string path, CFG &cfg, DDG &ddg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(cfg.m_label), Agdirected, NULL);
  render (agraph, cfg, ddg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, CFG &cfg, DDG &ddg)
{
  agattr (agraph, AGRAPH, "label", C(ddg.m_label));
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

  ListDigraph::NodeIt m (*graph);
  for (; m != INVALID; ++m)
    {
      set<ListDigraph::Node> *nodes = (*ddg.m_deps)[m];
      set<ListDigraph::Node>::iterator it = nodes -> begin ();
      for (; it != nodes -> end (); ++it)
	{
	  agedge (agraph, (*agnodes)[m], (*agnodes)[(*it)], NULL, TRUE);
	}
    }
}

void
Dot::toFile (string path, CFG &cfg, PDG &pdg)
{
  FILE *f = fopen (C(path), "w");
  Agraph_t *agraph = agopen (C(cfg.m_label), Agdirected, NULL);
  render (agraph, cfg, pdg);
  agwrite (agraph, f);
  
  agclose (agraph);
  fclose (f);
}

void
Dot::render (Agraph_t *agraph, CFG &cfg, PDG &pdg)
{
  agattr (agraph, AGRAPH, "label", C(pdg.m_label));
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
}
