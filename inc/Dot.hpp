#ifndef _DOT_HPP_
#define _DOT_HPP_

#include <string>
#include <graphviz/cgraph.h>
#include <lemon/list_graph.h>

class BB;
class CFG;
class ICFG;
class Dot {
public:
  static void toFile (std::string, BB   &);
  static void toFile (std::string, CFG  &);
  static void toFile (std::string, ICFG &);

private:
  Dot ();

  static void renderBB   (Agraph_t *, BB   &);
  static lemon::ListDigraph::NodeMap<Agnode_t *> *renderCFG  (Agraph_t *, CFG  &);
  static void renderICFG (Agraph_t *, ICFG &);
};

#endif // _DOT_HPP_
