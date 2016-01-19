#ifndef _DT_HPP_
#define _DT_HPP_

#include <lemon/list_graph.h>
#include <set>

class CFG;
class DT {
  friend class Dot;
  friend class CDG;

public:
  DT (const CFG *);
  
protected:
    std::             string                                           m_name;
    std::             string                                           m_label;

                const CFG                                             *m_cfg;
  lemon::ListDigraph::NodeMap<int>                                    *m_sdom;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node>               *m_idom;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node>               *m_ancestor;
  lemon::ListDigraph::NodeMap< std::set<lemon::ListDigraph::Node> * > *m_bucket;
  
private:
  lemon::ListDigraph::Node eval (lemon::ListDigraph::Node);
};

#endif // _DT_HPP_
