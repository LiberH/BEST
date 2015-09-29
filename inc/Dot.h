#ifndef _DOT_H_
#define _DOT_H_

#include "types.h"
#include <lemon/list_graph.h>
#include <graphviz/cgraph.h>

using namespace std;
using namespace lemon;

class CFG;
class Function;
class BasicBlock;
typedef ListDigraph::NodeMap<Agnode_t *> AgnodeMap;
typedef ListDigraph::NodeMap<string> LabelMap;

class Dot {
public:
  static string toDot (BasicBlock &);
  static string toDot (Function   &, LabelMap * = NULL);
  static string toDot (CFG        &);

private:
  Dot();

  static AgnodeMap *toDot_ (BasicBlock &, Agraph_t *);
  static AgnodeMap *toDot_ (Function   &, Agraph_t *, LabelMap * = NULL);
  static void       toDot_ (CFG        &, Agraph_t *);
};

#endif // _DOT_H_
