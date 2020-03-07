#include "CDG.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "PDT.hpp"

#include <cstdio>
#include <graphviz/cgraph.h>
#include <sstream>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

/* PUBLIC */

CDG::CDG (CFG *g, PDT *pdt)
{
  ostringstream ss_name, ss_label;
  ss_name  << "cdg";
  ss_label << "CDG(" << g -> m_label << ")";
  CFG *cfg = CFG::Reverse (g);

  m_name  = ss_label.str ();
  m_label = ss_label.str ();

  m_graph = new ListDigraph ();
  m_nodes = new map<int, ListDigraph::Node> ();
  m_bbs   = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  
  m_deps = new ListDigraph::NodeMap< vector<ListDigraph::Node> * > (*m_graph, NULL);
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      ListDigraph::Node m = this -> addBB (bb);
      (*m_deps)[m] = new vector<ListDigraph::Node> ();
    }

  ListDigraph::ArcIt a (*cfg -> m_graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node v = cfg -> m_graph -> source (a);
      ListDigraph::Node u = cfg -> m_graph -> target (a);
      if (u == v) // self loop ; post-dom itself
        continue;
      
      // TODO: clean the following code
      ListDigraph::Node i = (*pdt -> m_idom)[u];
      while (i != INVALID)
        {
          if (i == v) break;
          i = (*pdt -> m_idom)[i];
        }
      
      if (i == INVALID)
        {
          ListDigraph::Node cdg_v = (*m_nodes)[(*cfg -> m_bbs)[v] -> m_entry];
          ListDigraph::Node cdg_u = (*m_nodes)[(*cfg -> m_bbs)[u] -> m_entry];
          (*m_deps)[cdg_v] -> push_back (cdg_u);
          ListDigraph::Node pu = (*pdt -> m_idom)[u];
          ListDigraph::Node l = (*pdt -> m_idom)[v];
          while (l != INVALID)
            {
              if (l == u)
                {
                  ListDigraph::Node cdg_l = (*m_nodes)[(*cfg -> m_bbs)[l] -> m_entry];
                  (*m_deps)[cdg_l] -> push_back (cdg_l);
                  break;
                }
              
              if (l == pu)
                break;
              
              ListDigraph::Node cdg_l = (*m_nodes)[(*cfg -> m_bbs)[l] -> m_entry];
              ListDigraph::Node cdg_u = (*m_nodes)[(*cfg -> m_bbs)[u] -> m_entry];
              (*m_deps)[cdg_l] -> push_back (cdg_u);
              l = (*pdt -> m_idom)[l];
            }
        }
    }

  ListDigraph::NodeIt m (*m_graph);
  for (; m != INVALID; ++m)
    {
      vector<ListDigraph::Node>::iterator node_it = (*m_deps)[m] -> begin ();
      for (; node_it != (*m_deps)[m] -> end (); ++node_it)
        {
          ListDigraph::Node n = *node_it;
          BB *bb = (*m_bbs)[m];
          BB *cc = (*m_bbs)[n];
          this -> addEdge (bb, cc);
        }
    }
}

ListDigraph::Node
CDG::addBB (BB *bb)
{
  ListDigraph::Node n = m_graph -> addNode ();
  
  (*m_bbs)[n] = bb;
  vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();
  for (; inst_it != bb -> m_insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      (*m_nodes)[inst -> m_addr] = n;
    }
  
  return n;
}

ListDigraph::Arc
CDG::addEdge (BB *bb,
              BB *cc)
{
  ListDigraph::Node n = (*m_nodes)[bb -> m_entry];
  ListDigraph::Node m = (*m_nodes)[cc -> m_entry];
  return m_graph -> addArc (n, m);
}

// static
void
CDG::ToFile (string fn, CDG *cdg)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(cdg -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C(cdg -> m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cdg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);    
  Agnode_t *agn = agnode (agraph, "start", TRUE);
  agsafeset (agn, "label", "_start",  "error");

  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cdg -> m_bbs)[n];
      (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
      agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
      
      if ((*cdg -> m_deps)[n] -> empty ())
        agedge (agraph, agn, (*agnodes)[n], NULL, TRUE);
    }
  
  ListDigraph::NodeIt m (*cdg -> m_graph);
  for (; m != INVALID; ++m)
    {
      vector<ListDigraph::Node>::iterator node_it = (*cdg -> m_deps)[m] -> begin ();
      for (; node_it != (*cdg -> m_deps)[m] -> end (); ++node_it)
        {
          ListDigraph::Node n = *node_it;
          agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
        }
    }

  agwrite (agraph, f);  
  agclose (agraph);
  fclose (f);
}
