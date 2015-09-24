#include "DFS.h"

using namespace std;
using namespace lemon;

DistMap
*dfs (Function &f)
{
  Dfs<ListDigraph> dfs (*f.graph_);
  DistMap *dists = new DistMap (*f.graph_);

  dfs.run (f.entry_);
  ListDigraph::NodeIt n (*f.graph_);
  for (; n != INVALID; ++n)
    (*dists)[n] = dfs.dist (n);

  return dists;
}
