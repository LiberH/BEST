#ifndef _DFS_HPP_
#define _DFS_HPP_

#include <lemon/list_graph.h>

class CFG;
class DFS {
  friend class DT;
  friend class PDT;
  
public:
  DFS (const CFG *);
  static void ToFile (std::string, CFG *, DFS *);
  
protected:
  lemon::ListDigraph::NodeMap<int>                      *m_order;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_parent;
    std::             vector <lemon::ListDigraph::Node> *m_list;
};

#endif // _DFS_HPP_
