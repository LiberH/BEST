#ifndef _BB_HPP_
#define _BB_HPP_

#include "types.h"
#include <vector>

class Inst;
class BB {
  friend class Dot;
  friend class CFG;
  friend class DDG;
  
public:
  BB ();
  
  void addInst (Inst &);
  static std::vector<BB *> *FromFile (std::string          );
  static std::vector<u32 > *Leaders  (std::vector<Inst *> &);
  
protected:
    std::string          m_name;
    std::string          m_label;
  
    std::vector<Inst *> *m_insts;
         int             m_entry;

private:
  static int m_id;
};

#endif // _BB_HPP_
