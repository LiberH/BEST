#ifndef _CFG_HPP_
#define _CFG_HPP_

#include <string>
#include <lemon/list_graph.h>

class BB;
class CFG {
  friend class ICFG;
  friend class Dot;
  
public:
  CFG (std::string);

  lemon::ListDigraph::Node  addNode  (     BB     &);
  lemon::ListDigraph::Arc   addEdge  (     BB     &,
					   BB     &);
                      BB   *findNode (std::string &);

  lemon::ListDigraph::Node &entry () { return m_entry; };
  lemon::ListDigraph::Node &exit  () { return m_exit;  };

  void entry (lemon::ListDigraph::Node &n) { m_entry = n; };
  void exit  (lemon::ListDigraph::Node &n) { m_exit  = n; };

protected:
  lemon::ListDigraph::Node           m_id;
    std::             string         m_label;
    std::             string         m_name;
  
  lemon::             ListDigraph   *m_graph;
  lemon::ListDigraph::NodeMap<BB *> *m_bbs;
  lemon::ListDigraph::Node           m_entry;
  lemon::ListDigraph::Node           m_exit;
};

#endif // _CFG_HPP_
