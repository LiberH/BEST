#ifndef _CFG_HPP_
#define _CFG_HPP_

#include "types.h"
#include <lemon/list_graph.h>
#include <map>

class BB;
class CFG {
  friend class Dot;
  friend class ICFG;
  friend class DFS;
  friend class DT;
  friend class PDT;
  friend class CDG;
  friend class DDG;
  friend class PDG;

public:
  lemon::ListDigraph::Node addBB   (BB &);
  lemon::ListDigraph::Arc  addEdge (BB &, BB &);
  
  static CFG *Reverse (const CFG &);
  static CFG *FromFile (std::string);
  
protected:
    std::             string                             m_name;
    std::             string                             m_label;
  
  lemon::             ListDigraph                       *m_graph;
    std::             map<int,lemon::ListDigraph::Node> *m_nodes;
  lemon::ListDigraph::NodeMap<BB *>                     *m_bbs;
  lemon::ListDigraph::NodeMap< std::vector<BB *> * >    *m_targets;
  lemon::ListDigraph::Node                               m_entry;
  lemon::ListDigraph::Node                               m_exit;

private:
  static int m_id;
  
  CFG ();
  void findTargets (std::vector<BB *> &);
  void blr_patch ();
};

#endif // _CFG_HPP_
