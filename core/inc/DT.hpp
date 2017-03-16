#ifndef _DT_HPP_
#define _DT_HPP_

#include <lemon/list_graph.h>
#include <set>
#include <map>

class BB;
class CFG;
class DT {
  friend class CDG;

public:
  DT (const CFG *);
  
  lemon::ListDigraph::Node addBB   (const BB *);
  lemon::ListDigraph::Arc  addEdge (const BB *, const BB *);
  
  static void ToFile (std::string, DT *);
 
protected:
    std::             string                                           m_name;
    std::             string                                           m_label;

  lemon::             ListDigraph                                     *m_graph;
    std::             map<int,lemon::ListDigraph::Node>               *m_nodes;
  lemon::ListDigraph::NodeMap<const BB *>                             *m_bbs;
  
  lemon::ListDigraph::NodeMap<int>                                    *m_sdom;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node>               *m_idom;
  lemon::ListDigraph::NodeMap<lemon::ListDigraph::Node>               *m_ancestor;
  lemon::ListDigraph::NodeMap< std::set<lemon::ListDigraph::Node> * > *m_bucket;
  
private:
  lemon::ListDigraph::Node eval (lemon::ListDigraph::Node);
};

#endif // _DT_HPP_
