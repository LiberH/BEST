#ifndef _DFS_H_
#define _DFS_H_

#include "types.h"
#include "Dot.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Function;
typedef ListDigraph::NodeMap<int> DistMap;

class DFS {
private :
  DFS();
  
public:
  static DistMap  *dists  (Function &);
  static LabelMap *labels (Function &);
};
#endif // _DFS_H_
