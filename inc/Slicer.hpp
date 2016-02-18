#ifndef _SLICER_HPP_
#define _SLICER_HPP_

#include "types.h"
#include <vector>

class Inst;
class CFG;
class PDG;
class Slicer {
  
public:
  Slicer (CFG *, PDG *);
  std::vector<Inst *> *slice ();
  std::vector<Inst *> *slice (u32);
  std::vector<Inst *> *slice (std::vector<Inst *> *);
  
protected:
  const PDG *m_pdg;
  const CFG *m_cfg;
};

#endif // _SLICER_HPP_
