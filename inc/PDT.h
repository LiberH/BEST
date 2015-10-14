#ifndef _PDT_H_
#define _PDT_H_

#include <lemon/list_graph.h>

class CFG;
class DFTree;
class PDT {
  friend class Dot;

public:
  PDT (CFG &, DFTree &);

protected:
    std::string       m_label;
         CFG         *m_cfg;
         DFTree      *m_tree;
  lemon::ListDigraph *m_graph;

private:
  lemon::ListDigraph::Node eval (lemon::ListDigraph::Node);
};
#endif // _PDT_H_
