#include "CDG.hpp"

#include "CFG.hpp"
#include "PDT.hpp"
#include <sstream>

using namespace std;
using namespace lemon;

// TODO: fix special entry node
CDG::CDG (const CFG *cfg, const PDT *pdt)
{
  ostringstream ss_name, ss_label;
  ss_name  << "cdg";
  ss_label << "CDG(" << cfg -> m_label << ")";

  m_name  = ss_label.str ();
  m_label = ss_label.str ();

  m_cfg = pdt -> m_cfg;
  m_deps = new ListDigraph::NodeMap< vector<ListDigraph::Node> * > (*m_cfg -> m_graph, NULL);
  ListDigraph::NodeIt n (*m_cfg -> m_graph);
  for (; n != INVALID; ++n)
    (*m_deps)[n] = new vector<ListDigraph::Node> ();
  
  ListDigraph::ArcIt a (*m_cfg -> m_graph);
  for (; a != INVALID; ++a)
    {
      ListDigraph::Node v = m_cfg -> m_graph -> source (a);
      ListDigraph::Node u = m_cfg -> m_graph -> target (a);
      if (u == v) // self loop ; post-dom itself
	continue;

      ListDigraph::Node i = (*pdt -> m_idom)[u];
      while (i != INVALID)
	{
	  if (i == v) break;
	  i = (*pdt -> m_idom)[i];
	}
      
      if (i == INVALID)
	{
	  (*m_deps)[v] -> push_back (u);
	  ListDigraph::Node pu = (*pdt -> m_idom)[u];
	  ListDigraph::Node l = (*pdt -> m_idom)[v];
	  while (l != INVALID)
	    {
	      if (l == u)
		{
		  (*m_deps)[l] -> push_back (l);
		  break;
		}
	      
	      if (l == pu)
		break;
	      
	      (*m_deps)[l] -> push_back (u);
	      l = (*pdt -> m_idom)[l];
	    }
	}
    }
}
