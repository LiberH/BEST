#ifndef _PDG_HPP_
#define _PDG_HPP_

#include <lemon/list_graph.h>
#include <string>
#include <map>

class Inst;
class BB;
class CFG;
class CDG;
class DDG;
class PDG {
  friend class Slicer;
  
public:
  PDG (CFG *, CDG *, DDG *);
  
  lemon::ListDigraph::Node addBB   (BB   *);
  lemon::ListDigraph::Node addInst (Inst *);
  lemon::ListDigraph::Arc  addEdge (lemon::ListDigraph::Node, lemon::ListDigraph::Node);
  
  static void ToFile (std::string, PDG *);

protected:
    std::string                                          m_name;
    std::string                                          m_label;

  lemon::             ListDigraph                       *m_graph;
    std::             map<int,lemon::ListDigraph::Node> *m_nodes;
  
  lemon::ListDigraph::NodeMap<bool  >             *m_meta;
  lemon::ListDigraph::NodeMap<BB   *>             *m_bbs;
  lemon::ListDigraph::NodeMap<Inst *>             *m_insts;
};

#endif // _PDG_HPP_
