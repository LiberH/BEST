#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "types.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class BasicBlock;
class staticInfo;

class Instruction {
public:
  ListDigraph::Node id_;
  string label_;
  string name_;
  
  /* --- */
  u32 pc_;
  string mnemo_;
  
  u64 write_regs_;
  u64 read_regs_;
  
  bool is_branch_;
  bool is_unkown_;
  bool do_link_;
  bool is_uncond_;

  u64 target_;
  /* --- */
  
  Instruction (BasicBlock &, staticInfo &);
};

#endif // _INSTRUCTION_H_
