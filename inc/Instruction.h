#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "types.h"
#include <lemon/list_graph.h>

using namespace std;
using namespace lemon;

class Instruction {
  friend class BasicBlock;
  friend class Dot;
  
public:
  Instruction();
  
protected:
  Node   m_id;
  string m_name;
  string m_label;
  
  /*
  u32    m_pc;
  string m_mnemo;
  u64    m_write_regs;
  u64    m_read_regs;
  bool   m_is_branch;
  bool   m_is_unkown;
  bool   m_do_link;
  bool   m_is_uncond;
  u64    m_target;
  */
};

#endif // _INSTRUCTION_H_
