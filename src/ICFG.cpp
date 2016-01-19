#include "ICFG.hpp"
#include "CFG.hpp"

using namespace std;
using namespace lemon;

ICFG::ICFG ()
{
  ostringstream ss_name, ss_label;
  ss_name  << "icfg";
  ss_label << "ICFG";  

  m_name  = ss_name.str ();
  m_label = ss_label.str ();
  
  m_graph = new ListDigraph();
  m_cfgs  = new ListDigraph::NodeMap<CFG *> (*m_graph, NULL);
}

ListDigraph::Node
ICFG::addCFG (CFG &cfg)
{
  ListDigraph::Node n = m_graph -> addNode();
  cfg.m_name = m_name + cfg.m_name;
  
  (*m_cfgs)[n] = &cfg;  
  return n;
}
