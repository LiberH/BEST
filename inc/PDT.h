#ifndef _PDT_H_
#define _PDT_H_

#include <lemon/list_graph.h>

class CFG;
class DFSTree;
class PDT {
  friend class Dot;

public:
  PDT (CFG &, DFSTree &);

protected:
    std::             string                             m_label;
                      CFG                               *m_cfg;
                      DFSTree                           *m_tree;
  lemon::             ListDigraph                       *m_graph;
  lemon::ListDigraph::NodeMap<int>                      *m_semi;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_ancestor;
  
private:
  lemon::ListDigraph::Node eval (lemon::ListDigraph::Node);
};
#endif // _PDT_H_
