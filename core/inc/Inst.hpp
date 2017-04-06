#ifndef _INST_HPP_
#define _INST_HPP_

#include "types.h"
#include <string>
#include <vector>

class staticInfo;
class Inst {
  friend class BB;
  friend class CFG;
  friend class DT;
  friend class CDG;
  friend class DDG;
  friend class PDG;
  friend class Slicer;
  
public:
  Inst (const staticInfo &);
  Inst (const Inst &);
  static      void FromFile  (std::string, u32 *, u32 *, std::vector<Inst *> **, u32 *, std::vector<s32> **, u32 *, std::vector<s32> **);
  static      void ToFile    (std::string, std::vector<Inst *> *);
  static      int  CountRegs (std::vector<Inst *> *);
  
protected:
         int     m_num;
    std::string  m_name;
    std::string  m_label;
  
         u32     m_addr;
    std::string  m_disass;
    std::string  m_function;
         u64     m_refs;
         u64     m_defs;
         Inst   *m_prev;
         Inst   *m_next;
  
         bool    m_branch;
         u32     m_test;
         int     m_crfD;
         bool    m_unknown;
         bool    m_link;
         bool    m_uncond;
         u32     m_target;

         bool    m_memory;

         int     m_x;
         int     m_y;
  
private:
  static int m_id;
  static bool byAddr (const Inst *, const Inst *);
};

#endif // _INST_HPP_
