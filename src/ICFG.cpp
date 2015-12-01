#include "ICFG.hpp"
#include "CFG.hpp"

using namespace lemon;
using namespace std;

ICFG::ICFG (string label)
{
  m_label = label;
  
  m_graph = new ListDigraph();
  m_cfgs  = new ListDigraph::NodeMap<CFG *> (*m_graph, NULL);
}

ListDigraph::Node
ICFG::addNode (CFG &cfg)
{
  ListDigraph::Node n = m_graph -> addNode();
  (*m_cfgs)[n] = &cfg;
  cfg.m_id = n;
  
  cfg.m_name = m_label + cfg.m_label;
  return cfg.m_id;
}

CFG *
ICFG::findNode (string &s)
{
  CFG *cfg;
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      cfg = (*m_cfgs)[n];
      if (cfg -> m_label == s)
	break;
    }

  return cfg;
}
