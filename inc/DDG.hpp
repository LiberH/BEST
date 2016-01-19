#ifndef _DDG_HPP_
#define _DDG_HPP_

#include <set>
#include <lemon/list_graph.h>

class Inst;
class CFG;
class DDG {
  friend class Dot;
  friend class PDT;
  
public:
  DDG (CFG &);

protected:
    std::             string                                             m_name;
    std::             string                                             m_label;
  
    std::             vector < std::set<                    Inst *> * > *m_kills;
  lemon::ListDigraph::NodeMap< std::set<                    Inst *> * > *m_gens;
  lemon::ListDigraph::NodeMap< std::set<                    Inst *> * > *m_ins;
  lemon::ListDigraph::NodeMap< std::set<                    Inst *> * > *m_outs;
  lemon::ListDigraph::NodeMap< std::set<lemon::ListDigraph::Node  > * > *m_deps;
};

#endif // _DDG_HPP_
