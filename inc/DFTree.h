#ifndef _DFTREE_H_
#define _DFTREE_H_

#include <lemon/list_graph.h>

class CFG;
class DFTree {
  friend class Dot;
  
public:
  DFTree (const CFG &);

protected:
    std::             string                             m_label;
  const               CFG                               *m_cfg;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_ref;
  lemon::             ListDigraph                       *m_graph;
  lemon::ListDigraph::Node                               m_root;
};

#endif // _DFTREE_H_
