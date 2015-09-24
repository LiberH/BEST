#ifndef _DFS_H_
#define _DFS_H_

#include "Function.h"
#include <lemon/list_graph.h>
#include <lemon/dfs.h>

using namespace std;
using namespace lemon;

typedef ListDigraph::NodeMap<int> DistMap;
DistMap *dfs (Function &);

#endif // _DFS_H_
