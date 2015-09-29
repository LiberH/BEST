#include "DFS.h"
#include "BasicBlock.h"
#include "Function.h"

#include <lemon/dfs.h>
#include <iostream>

using namespace std;
using namespace lemon;

DistMap *
DFS::dists (Function &f)
{
  Dfs<Graph> dfs (*f.m_graph);
  DistMap *dists = new DistMap (*f.m_graph);

  dfs.run (f.m_entry);
  NodeIt n (*f.m_graph);
  for (; n != INVALID; ++n)
    (*dists)[n] = dfs.dist (n);

  return dists;
}

LabelMap *
DFS::labels (Function &f)
{
  Dfs<Graph> dfs (*f.m_graph);
  LabelMap *labels = new LabelMap (*f.m_graph);

  dfs.run (f.m_entry);
  NodeIt n (*f.m_graph);
  for (; n != INVALID; ++n)
    {
      ostringstream oss;
      oss << dfs.dist (n);
      (*labels)[n] = oss.str();
    }
  
  return labels;
}
