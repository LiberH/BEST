#ifndef _CFG_HPP_
#define _CFG_HPP_

#include "types.h"
#include <lemon/list_graph.h>
#include <map>

class BB;
class CFG {
  friend class ICFG;
  friend class DFS;
  friend class DT;
  friend class PDT;
  friend class CDG;
  friend class DDG;
  friend class PDG;

public:
  CFG ();
  
  lemon::ListDigraph::Node          addBB   (BB &);
  lemon::ListDigraph::Arc           addEdge (BB &, BB &);
    std::             vector<BB *> *bbs     ();
  
  static CFG  *Reverse  (const CFG *);
  static CFG  *FromFile (std::string);
  static void  ToFile   (std::string, CFG *);
  
protected:
    std::             string                             m_name;
    std::             string                             m_label;
  
  lemon::             ListDigraph                       *m_graph;
    std::             map<int,lemon::ListDigraph::Node> *m_nodes;
  lemon::ListDigraph::NodeMap<BB *>                     *m_bbs;
  
  lemon::ListDigraph::NodeMap< std::vector<BB *> * >    *m_preds;
  lemon::ListDigraph::NodeMap< std::vector<BB *> * >    *m_succs;
  lemon::ListDigraph::Node                               m_entry;
  lemon::ListDigraph::Node                               m_exit;

private:
  void findSuccs (std::vector<BB *> &);
  void blr_patch ();
};

#endif // _CFG_HPP_
