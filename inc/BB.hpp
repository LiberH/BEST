#ifndef _BB_HPP_
#define _BB_HPP_

#include "types.h"
#include <vector>

class Inst;
class BB {
  friend class CFG;
  friend class DFS;
  friend class DT;
  friend class CDG;
  friend class DDG;
  friend class PDG;
  friend class Slicer;
  
public:
  BB ();
  
  void addInst (Inst &);
  static std::vector<BB *> *FromFile (std::string, u32 *, u32 *);
  static      void          ToFile   (std::string, std::vector<BB *> *);
  static      void          ToFile   (std::string, BB *);
  static std::vector<u32 > *Leaders  (std::vector<Inst *> &);
  static bool byAddr (const BB *, const BB *);
  
protected:
    std::string          m_name;
    std::string          m_label;
  
    std::vector<Inst *> *m_insts;
         u32             m_entry;

private:
  static int m_id;
};

#endif // _BB_HPP_
