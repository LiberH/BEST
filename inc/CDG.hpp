#ifndef _CDG_HPP_
#define _CDG_HPP_

#include <lemon/list_graph.h>

class CFG;
class PDT;
class CDG {
  friend class Dot;

public:
  CDG (const CFG *, const PDT *);

protected:
    std::             string                                              m_name;
    std::             string                                              m_label;

                const CFG                                                *m_cfg;
  lemon::ListDigraph::NodeMap< std::vector<lemon::ListDigraph::Node> * > *m_deps;
};

#endif // _CDG_HPP_
