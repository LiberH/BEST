#ifndef _DDG_HPP_
#define _DDG_HPP_

#include <lemon/list_graph.h>
#include <set>
#include <map>

class Inst;
class CFG;
class DDG {
  friend class PDT;
  friend class PDG;
  
public:
  DDG (CFG *);
  static void ToFile (std::string, CFG *, DDG *);
  
protected:
  std::vector<      std::set<Inst *> * > *m_kills;
  std::map   < int, std::set<Inst *> * > *m_gens;
  std::map   < int, std::set<Inst *> * > *m_ins;
  std::map   < int, std::set<Inst *> * > *m_outs;
  std::map   < int, std::set<Inst *> * > *m_deps;
};

#endif // _DDG_HPP_
