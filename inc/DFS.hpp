#ifndef _DFSTREE_H_
#define _DFSTREE_H_

#include <lemon/list_graph.h>

class CFG;
class DFSTree {
  friend class PDT;
  friend class Dot;
  
public:
  DFSTree (const DFSTree &);
  DFSTree (const CFG     &);

  static DFSTree *tree (const DFSTree &);
  
protected:
          std::             string                             m_label;
  const                     CFG                               *m_cfg;
        lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_ref;
          std::             vector<lemon::ListDigraph::Node>  *m_list;
        lemon::             ListDigraph                       *m_graph;
        lemon::ListDigraph::Node                               m_root;
        lemon::ListDigraph::NodeMap<int>                      *m_order;
        lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_parent;
};

#endif // _DFSTREE_H_
