#include "DFS.hpp"

#include "CFG.hpp"
#include <lemon/dfs.h>
#include <sstream>

using namespace std;
using namespace lemon;

DFS::DFS (const CFG &cfg)
{
  ostringstream ss_name, ss_label;
  ss_name  << "dfs";
  ss_label << "DFS(" << cfg.m_label << ")";

  m_name  = ss_name.str ();
  m_label = ss_label.str ();
  
  int c = countNodes (*cfg.m_graph);
  Dfs<ListDigraph> dfs (*cfg.m_graph);
  m_order  = new ListDigraph::NodeMap<int> (*cfg.m_graph, -1);
  m_parent = new ListDigraph::NodeMap<ListDigraph::Node> (*cfg.m_graph, INVALID);    
  m_list   = new vector<ListDigraph::Node> (c, INVALID);

  int i = 0;
  dfs.init ();
  dfs.addSource (cfg.m_entry);
  (*m_order )[cfg.m_entry] = i;
  (*m_parent)[cfg.m_entry] = INVALID;
  (*m_list  )[i]           = cfg.m_entry;
  while (!dfs.emptyQueue ())
    {
      ListDigraph::Arc  a = dfs.nextArc ();
      ListDigraph::Node u = cfg.m_graph -> source (a);
      ListDigraph::Node v = cfg.m_graph -> target (a);
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
