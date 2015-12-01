#ifndef _ICFG_HPP_
#define _ICFG_HPP_

#include <lemon/list_graph.h>

class CFG;
class ICFG {
  friend class Dot;
  
public:
  ICFG (std::string);
  
  lemon::ListDigraph::Node  addNode  (     CFG    &);  
                      CFG  *findNode (std::string &);

protected:
    std::             string          m_label;
  
  lemon::             ListDigraph    *m_graph;
  lemon::ListDigraph::NodeMap<CFG *> *m_cfgs;
};

#endif // _ICFG_HPP_
