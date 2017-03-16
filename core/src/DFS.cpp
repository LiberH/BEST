#include "DFS.hpp"

#include "BB.hpp"
#include "CFG.hpp"
#include <cstdio>
#include <graphviz/cgraph.h>
#include <lemon/dfs.h>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

/* PUBLIC */

DFS::DFS (const CFG *cfg)
{
  int c = countNodes (*cfg -> m_graph);
  Dfs<ListDigraph> dfs (*cfg -> m_graph);
  m_order  = new ListDigraph::NodeMap<int> (*cfg -> m_graph, -1);
  m_parent = new ListDigraph::NodeMap<ListDigraph::Node> (*cfg -> m_graph, INVALID);    
  m_list   = new vector<ListDigraph::Node> (c, INVALID);

  int i = 0;
  dfs.init ();
  dfs.addSource (cfg -> m_entry);
  (*m_order )[cfg -> m_entry] = i;
  (*m_parent)[cfg -> m_entry] = INVALID;
  (*m_list  )[i]              = cfg -> m_entry;
  while (!dfs.emptyQueue ())
    {
      ListDigraph::Arc  a = dfs.nextArc ();
      ListDigraph::Node u = cfg -> m_graph -> source (a);
      ListDigraph::Node v = cfg -> m_graph -> target (a);
      if (!dfs.reached (v))
	{
	  i++;
	  
	  (*m_order )[v] = i;
	  (*m_parent)[v] = u;
	  (*m_list  )[i] = v;
	}
      
      dfs.processNextArc ();
    }
}

// static
void
DFS::ToFile (string fn, CFG *cfg, DFS *dfs)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(cfg -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C("DFS(" + cfg -> m_label + ")"));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);  
  
  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      ostringstream oss;
      oss << (*dfs -> m_order)[n];
      agsafeset ((*agnodes)[n], "label", C(oss.str ()), "error");
    }
  
  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
    }

  agwrite (agraph, f);  
  agclose (agraph);
  fclose (f);

}
