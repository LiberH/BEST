#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <lemon/list_graph.h>

using namespace std;

class Instruction {
public:
  string name_;
  
  Instruction (string &s);
};

#endif // _INSTRUCTION_H_
