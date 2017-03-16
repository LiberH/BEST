#ifndef _PDT_HPP_
#define _PDT_HPP_

#include "DT.hpp"

class CFG;
class PDT: public DT {
  friend class CDG;
  
public:
  PDT (const CFG *);
};

#endif // _PDT_HPP_
