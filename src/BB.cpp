#include "BB.hpp"
#include "Inst.hpp"

using namespace lemon;
using namespace std;

BB::BB (string label)
{
  m_id    = INVALID;
  m_name  = string ();
  m_label = label;
  
  m_instrs = new vector<Inst *> ();
  m_call   = NULL;
  m_ret    = INVALID;
}

void
BB::addNode (Inst &i)
{
  m_instrs -> push_back (&i);
  i.m_name  = m_label + i.m_label;
}
