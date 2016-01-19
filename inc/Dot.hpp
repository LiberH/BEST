#ifndef _DOT_HPP_
#define _DOT_HPP_

#include <string>
#include <graphviz/cgraph.h>
#include <lemon/list_graph.h>

class BB;
class CFG;
class ICFG;
class DFS;
class DT;
class CDG;
class DDG;
class PDG;
class Dot {
public:
  static void toFile (std::string, BB   &);
  static void toFile (std::string, CFG  &);
//static void toFile (std::string, ICFG &);
  static void toFile (std::string, CFG  &, DFS &);
  static void toFile (std::string, DT   &);
  static void toFile (std::string, CDG  &);
  static void toFile (std::string, CFG  &, DDG &);
  static void toFile (std::string, CFG  &, PDG &);

private:
  Dot ();

  static void render (Agraph_t *, BB   &);
  static void render (Agraph_t *, CFG  &);
//static void render (Agraph_t *, ICFG &);
  static void render (Agraph_t *, CFG  &, DFS &);
  static void render (Agraph_t *, DT  &);
  static void render (Agraph_t *, CDG &);
  static void render (Agraph_t *, CFG  &, DDG &);
  static void render (Agraph_t *, CFG  &, PDG &);
};

#endif // _DOT_HPP_
