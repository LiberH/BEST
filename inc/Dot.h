#ifndef _DOT_H_
#define _DOT_H_

#include <lemon/list_graph.h>
#include <graphviz/cgraph.h>

class ICFG;
class PDT;
class DFTree;
class CFG;
class BB;

class Dot {
public:
  static std::string toDot (BB     &);
  static std::string toDot (CFG    &);
  static std::string toDot (DFTree &);
  static std::string toDot (PDT    &);
  static std::string toDot (ICFG   &);

private:
  Dot ();

  static lemon::ListDigraph::NodeMap<Agnode_t *> *toDot_ (BB     &, Agraph_t *);
  static lemon::ListDigraph::NodeMap<Agnode_t *> *toDot_ (CFG    &, Agraph_t *);
  static lemon::ListDigraph::NodeMap<Agnode_t *> *toDot_ (DFTree &, Agraph_t *);
  static lemon::ListDigraph::NodeMap<Agnode_t *> *toDot_ (PDT    &, Agraph_t *);
  static                     void                 toDot_ (ICFG   &, Agraph_t *);
};

#endif // _DOT_H_
