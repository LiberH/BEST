#ifndef _PDT_H_
#define _PDT_H_

#include <lemon/list_graph.h>
#include <set>

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
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_ref;
  lemon::ListDigraph::NodeMap<int>                      *m_semi;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_idom;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node> *m_ancestor;
  lemon::ListDigraph::NodeMap< std::set<lemon::ListDigraph::Node> > *m_bucket;
  
private:
  lemon::ListDigraph::Node eval (lemon::ListDigraph::Node);
};
#endif // _PDT_H_
