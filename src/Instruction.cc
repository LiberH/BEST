#include "Instruction.h"
#include "BasicBlock.h"
#include "staticInfo.h"
#include <ostream>

using namespace std;
using namespace lemon;

Instruction::Instruction (BasicBlock &bb, staticInfo &si)
{
  ostringstream oss;
  
  id_         = INVALID;
  oss        << hex << pc_;
  label_      = oss.str();
  oss        << bb.name_ << "_" << hex << pc_;
  name_       = oss.str();
  
  pc_         = si.pc;
  mnemo_      = si.mnemo;
  write_regs_ = si.write_regs;
  read_regs_  = si.read_regs;
  is_branch_  = si.is_branch;
  is_unkown_  = si.is_unkown;
  do_link_    = si.do_link;
  is_uncond_  = si.is_uncond;
  target_     = si.target;
}
