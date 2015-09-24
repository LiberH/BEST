#ifndef _DOT_H_
#define _DOT_H_

#include "BasicBlock.h"
#include "Function.h"
#include "CFG.h"

using namespace std;

class BasicBlock;
class Function;
class CFG;
typedef ListDigraph::NodeMap<string> LabelMap;

class Dot {
private:
       Dot       ();
      ~Dot       ();
       Dot       (const Dot &);
  Dot &operator= (const Dot &);
  
public:
  static string toDot (BasicBlock &);
  static string toDot (Function &);
  static string toDot (Function &, LabelMap &);
  static string toDot (CFG &);
};

#endif // _DOT_H_
