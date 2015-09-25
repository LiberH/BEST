#ifndef _DOT_H_
#define _DOT_H_

#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class CFG;
class Function;
class BasicBlock;
typedef ListDigraph::NodeMap<string> LabelMap;

class Dot {
private:
       Dot       ();
      ~Dot       ();
       Dot       (const Dot &);
  Dot &operator= (const Dot &);
  
public:
  static string    toDot    (BasicBlock &);
  static string    toDot    (Function &);
  static string    toDot    (Function &, LabelMap &);
  static string    toDot    (CFG &);
  
  template <typename T>
  static LabelMap *toLabels (ListDigraph &, ListDigraph::NodeMap<T> *);
};

/* --- */

template <typename T>
LabelMap *
Dot::toLabels (ListDigraph &g, ListDigraph::NodeMap<T> *map)
{
  LabelMap *labels = new LabelMap (g);
  ListDigraph::NodeIt n (g);
  for (; n != INVALID; ++n)
    {
      ostringstream oss;
      oss << (*map)[n];
      (*labels)[n] = oss.str ();
    }
  
  return labels;
}

#endif // _DOT_H_
