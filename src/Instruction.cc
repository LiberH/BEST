#include "Instruction.h"

using namespace std;

Instruction::Instruction (BasicBlock &bb, string s)
{
  id_    = INVALID;
  label_ = s;
  name_  = bb.name_ + "_" + label_;
}
