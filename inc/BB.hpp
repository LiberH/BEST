#ifndef _BB_HPP_
#define _BB_HPP_

#include <string>
#include <lemon/list_graph.h>

class Inst;
class CFG;
class BB {
  friend class CFG;
  friend class Dot;
  
public:
  BB (std::string);
  void addNode (Inst &);
  void addEdge (Inst &, Inst &);

                      CFG  *call () { return m_call; };
  lemon::ListDigraph::Node &ret  () { return m_ret;  };

  void call (                    CFG  &c) { m_call = &c; };
  void ret  (lemon::ListDigraph::Node &n) { m_ret  =  n; };
  
protected:
  lemon::ListDigraph::Node            m_id;
    std::             string          m_name;
    std::             string          m_label;
  
    std::             vector<Inst *> *m_instrs;
                      CFG            *m_call;
  lemon::ListDigraph::Node            m_ret;
};

#endif // _BB_HPP_
