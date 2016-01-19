#ifndef _PDG_HPP_
#define _PDG_HPP_

#include <string>

class CFG;
class CDG;
class DDG;
class PDG {
  friend class Dot;
  
public:
  PDG (CFG &, CDG &, DDG &);

protected:
  std::string m_name;
  std::string m_label;
};

#endif // _PDG_HPP_
