#include "DT.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "DFS.hpp"

#include <cstdio>
#include <graphviz/cgraph.h>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

/* PUBLIC */

DT::DT (const CFG *cfg)
{
  ostringstream ss_name, ss_label;
  ss_name  << "dt";
  ss_label << "DT(" << cfg -> m_label << ")";

  m_name  = ss_name.str ();
  m_label = ss_label.str ();

  m_graph    = new ListDigraph ();
  m_nodes    = new map<int, ListDigraph::Node> ();
  m_bbs      = new ListDigraph::NodeMap<const BB *> (*m_graph, NULL);
  
  m_sdom     = new ListDigraph::NodeMap<int> (*cfg -> m_graph, 0);
  m_idom     = new ListDigraph::NodeMap<ListDigraph::Node> (*cfg -> m_graph, INVALID);
  m_ancestor = new ListDigraph::NodeMap<ListDigraph::Node> (*cfg -> m_graph, INVALID);
  m_bucket   = new ListDigraph::NodeMap< set<ListDigraph::Node> * > (*cfg -> m_graph, NULL);

  DFS *dfs = new DFS (cfg);
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      (*m_sdom  )[n] = (*dfs -> m_order)[n];
      (*m_bucket)[n] = new set<ListDigraph::Node> ();
      BB *bb = (*cfg -> m_bbs)[n];
      this -> addBB (bb);
    }
  
  // Step 2: Iteratively find the semi-dominators
  set<ListDigraph::Node>::iterator v;
  int count = countNodes (*cfg -> m_graph);
  for (int i = count -1; i > 0; --i)
    {
      ListDigraph::Node w = (*dfs -> m_list)[i];
      if (w == INVALID) continue;
      
      ListDigraph::InArcIt a (*cfg -> m_graph, w);
      for (; a != INVALID; ++a)
        {
          ListDigraph::Node v = cfg -> m_graph -> source (a);
          ListDigraph::Node u = eval (v);
          if ((*m_sdom)[u] < (*m_sdom)[w])
            (*m_sdom)[w] = (*m_sdom)[u];
        }

      (*m_bucket)[(*dfs -> m_list)[(*m_sdom)[w]]] -> insert (w); // add w to bucket(vertex(semi(w)))
      (*m_ancestor)[w] = (*dfs -> m_parent)[w]; // link

      // Step 3:
      v = (*m_bucket)[(*dfs -> m_parent)[w]] -> begin();
      for (; v != (*m_bucket)[(*dfs -> m_parent)[w]] -> end(); ++v)
        {
          (*m_bucket)[(*dfs -> m_parent)[w]] -> erase (v);
          ListDigraph::Node u = eval(*v);
          if ((*m_sdom)[u] < (*m_sdom)[*v])
            (*m_idom)[*v] = u;
          else
            (*m_idom)[*v] = (*dfs -> m_parent)[w];
        }
    }
  
  // Step 4:
  for (int i = 0; i < count ; ++i)
    {
      ListDigraph::Node w = (*dfs -> m_list)[i];
      if ((*dfs -> m_order)[(*m_idom)[w]] != (*m_sdom)[w])
        (*m_idom)[w] = (*m_idom)[(*m_idom)[w]];
    }

  ListDigraph::NodeIt m (*cfg -> m_graph);
  for (; m != INVALID; ++m)
    {
      ListDigraph::Node n = (*m_idom)[m];
      if (n == INVALID) continue;

      const BB *bb = (*cfg -> m_bbs)[n];
      const BB *cc = (*cfg -> m_bbs)[m];
      this -> addEdge (bb, cc);
    }
}

ListDigraph::Node
DT::addBB (const BB *bb)
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
DT::addEdge (const BB *bb,
             const BB *cc)
{
  ListDigraph::Node n = (*m_nodes)[bb -> m_entry];
  ListDigraph::Node m = (*m_nodes)[cc -> m_entry];
  return m_graph -> addArc (n, m);
}

// static
void
DT::ToFile (string fn, DT *dt)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(dt -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C(dt -> m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = dt -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);

  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      const BB *bb = (*dt -> m_bbs)[n];
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
  
  agwrite (agraph, f);
  agclose (agraph);
  fclose (f);
}

/* PRIVATE */

ListDigraph::Node
DT::eval (ListDigraph::Node v)
{
  ListDigraph::Node u = v;
  while ((*m_ancestor)[v] != INVALID)
    {
      if ((*m_sdom)[v] < (*m_sdom)[u])
        u = v;
      
      v = (*m_ancestor)[v];
    }
  
  return u;
}
