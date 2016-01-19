#ifndef _DFS_HPP_
#define _DFS_HPP_

#include <lemon/list_graph.h>

class CFG;
class DFS {
  friend class Dot;
  friend class DT;
  friend class PDT;
  
public:
  DFS (const CFG &);
  
protected:
    std::             string                             m_name;
    std::             string                             m_label;
  
  lemon::ListDigraph::NodeMap<int>                      *m_order;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_parent;
    std::             vector <lemon::ListDigraph::Node> *m_list;
};

#endif // _DFS_HPP_
