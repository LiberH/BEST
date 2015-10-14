#ifndef _INST_H_
#define _INST_H_

#include <lemon/list_graph.h>

class Inst {
  friend class BB;
  friend class Dot;
  
public:
  Inst ();
  
protected:
  lemon::ListDigraph::Node   m_id;
    std::             string m_name;
    std::             string m_label;
  
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

#endif // _INST_H_
