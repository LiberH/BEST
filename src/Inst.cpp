#include "Inst.hpp"

using namespace lemon;
using namespace std;

Inst::Inst (string label)
{
  m_id    = INVALID;
  m_name  = string ();
  m_label = label;
  
  /*
  m_pc         = si.pc;
  m_mnemo      = si.mnemo;
  m_write_regs = si.write_regs;
  m_read_regs  = si.read_regs;
  m_is_branch  = si.is_branch;
  m_is_unkown  = si.is_unkown;
  m_do_link    = si.do_link;
  m_is_uncond  = si.is_uncond;
  m_target     = si.target;
  */
}
