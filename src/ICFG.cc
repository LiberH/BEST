#include "ICFG.h"
#include "CFG.h"

using namespace lemon;
using namespace std;

ICFG::ICFG (string &s)
{
  m_label = s;
  m_graph = new ListDigraph();
  m_cfgs  = new ListDigraph::NodeMap<CFG *> (*m_graph, NULL);
}

ListDigraph::Node
ICFG::addNode (CFG &cfg)
{
  ListDigraph::Node n = m_graph -> addNode();
  (*m_cfgs)[n] = &cfg;
  cfg.m_id = n;
  
  ostringstream oss;
  oss << "F" << m_graph -> id (cfg.m_id);
  cfg.m_label = oss.str();
  cfg.m_name = cfg.m_label;

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
