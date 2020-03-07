#include "PDG.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "CDG.hpp"
#include "DDG.hpp"
#include <sstream>

#include <cstdio>
#include <graphviz/cgraph.h>
#include <lemon/maps.h>
#include <sstream>

#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;

/* PUBLIC */

PDG::PDG (CFG *cfg, CDG *cdg, DDG *ddg)
{
  ostringstream ss_name, ss_label;
  ss_name  << "pdg";
  ss_label << "PDG(" << cfg -> m_label << ")";

  m_name  = ss_label.str ();
  m_label = ss_label.str ();

  m_graph = new ListDigraph ();
  m_nodes = new map<int, ListDigraph::Node> ();
  m_meta  = new ListDigraph::NodeMap<bool  > (*m_graph, false);
  m_bbs   = new ListDigraph::NodeMap<BB   *> (*m_graph, NULL);
  m_insts = new ListDigraph::NodeMap<Inst *> (*m_graph, NULL);

  ListDigraph::NodeIt n (*cdg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cdg -> m_bbs)[n];
      ListDigraph::Node m = this -> addBB (bb);

      vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();
      for (; inst_it != bb -> m_insts -> end (); ++inst_it)
        {
          Inst *inst = *inst_it;
          ListDigraph::Node i = this -> addInst (inst);
          this ->  addEdge (m, i);
        }
    }

  ListDigraph::ArcIt a (*cdg -> m_graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node u = cdg -> m_graph -> source (a);
      ListDigraph::Node v = cdg -> m_graph -> target (a);
      
      BB *bb = (*cdg -> m_bbs)[v];
      BB *cc = (*cdg -> m_bbs)[u];
      this -> addEdge ((*m_nodes)[bb -> m_entry], (*m_nodes)[cc -> m_entry]);
    }

  ListDigraph::NodeIt m (*cdg -> m_graph);
  for (; m != INVALID; ++m)
    {
      BB *bb = (*cdg -> m_bbs)[m];
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it  = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
        {
          Inst *inst = *inst_it;
          set<Inst *> *deps = (*ddg -> m_deps)[inst -> m_addr];
          set<Inst *>::iterator dep_it  = deps -> begin ();
          for (; dep_it != deps -> end (); ++dep_it)
            {
              Inst *dep = *dep_it;
              this -> addEdge ((*m_nodes)[dep -> m_addr +1], (*m_nodes)[inst -> m_addr +1]);
            }
        }
    }

}

ListDigraph::Node
PDG::addBB (BB *bb)
{
  ListDigraph::Node n = m_graph -> addNode ();
  
  (*m_meta)[n] = false;
  (*m_bbs )[n] = bb;
  vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();
  for (; inst_it != bb -> m_insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      (*m_nodes)[inst -> m_addr] = n;
    }
  
  return n;
}

ListDigraph::Node
PDG::addInst (Inst *inst)
{
  ListDigraph::Node n = m_graph -> addNode ();
  (*m_meta )[n] = true;
  (*m_insts)[n] = inst;
  (*m_nodes)[inst -> m_addr +1] = n;
  
  return n;
}

ListDigraph::Arc
PDG::addEdge (ListDigraph::Node n, ListDigraph::Node m)
{
  return m_graph -> addArc (n, m);
}
  
// static
void
PDG::ToFile (string fn, PDG *pdg)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(pdg -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C(pdg -> m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = pdg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);  
  Agnode_t *agn = agnode (agraph, "start", TRUE);
  agsafeset (agn, "label", "_start",       "error");

  ListDigraph::NodeIt n (*graph);
  for (; n != INVALID; ++n)
    {
      if (!(*pdg -> m_meta)[n])
        {
          BB *bb = (*pdg -> m_bbs)[n];
          (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
          agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");

          InDegMap<ListDigraph> idm (*graph);
          if (!idm[n])
            {
              Agedge_t *age = agedge (agraph, agn, (*agnodes)[n], NULL, TRUE);
              agsafeset (age, "style", "bold", "");
            }
        }
      else
        {
          ostringstream oss;
          Inst *inst = (*pdg -> m_insts)[n];
          oss << hex << inst -> m_addr;
          (*agnodes)[n] = agnode (agraph, C(inst -> m_name), TRUE);
          agsafeset ((*agnodes)[n], "label", C(oss.str ()), "error");
          agsafeset ((*agnodes)[n], "ranksep", "0.2",   "");  
          agsafeset ((*agnodes)[n], "shape",   "box",   "");  
          agsafeset ((*agnodes)[n], "height",  "0.3",   "");  
        }
    }

  ListDigraph::ArcIt a (*graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node n = graph -> source (a);
      ListDigraph::Node m = graph -> target (a);
      Agedge_t *age = agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
      
      if ((*pdg -> m_meta)[n] && (*pdg -> m_meta)[m])
        agsafeset (age, "style", "dashed", "");
      
      if (!(*pdg -> m_meta)[n] && !(*pdg -> m_meta)[m])
        agsafeset (age, "style", "bold", "");
    }

  agwrite (agraph, f);  
  agclose (agraph);
  fclose (f);
}
