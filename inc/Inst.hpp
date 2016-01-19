#ifndef _INST_HPP_
#define _INST_HPP_

#include "types.h"
#include <string>
#include <vector>

class staticInfo;
class Inst {
  friend class Dot;
  friend class BB;
  friend class CFG;
  friend class DDG;
  
public:
  Inst (const staticInfo &);
  static std::vector<Inst *> *FromFile (std::string);

protected:
    std::string  m_name;
    std::string  m_label;
  
         u32     m_addr;
    std::string  m_disass;
         u64     m_refs;
         u64     m_defs;
         Inst   *m_next;
  
         bool    m_branch;
         bool    m_unknown;
         bool    m_link;
         bool    m_uncond;
         u32     m_target;
  
private:
  static int m_id;
};

#endif // _INST_HPP_
