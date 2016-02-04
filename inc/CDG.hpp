#ifndef _CDG_HPP_
#define _CDG_HPP_

#include <lemon/list_graph.h>
#include <map>

class BB;
class CFG;
class PDT;
class CDG {
  friend class PDG;

public:
  CDG (CFG *, PDT *);
  
  lemon::ListDigraph::Node addBB   (BB *);
  lemon::ListDigraph::Arc  addEdge (BB *, BB *);
  
  static void ToFile (std::string, CDG *);
  
protected:
    std::             string                                              m_name;
    std::             string                                              m_label;

  lemon::             ListDigraph                                        *m_graph;
    std::             map<int,lemon::ListDigraph::Node>                  *m_nodes;
  lemon::ListDigraph::NodeMap<BB *>                                *m_bbs;
  lemon::ListDigraph::NodeMap< std::vector<lemon::ListDigraph::Node> * > *m_deps;
};

#endif // _CDG_HPP_
