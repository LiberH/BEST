#ifndef _BB_H_
#define _BB_H_

#include <lemon/list_graph.h>

class CFG;
class Inst;
class BB {
  friend class CFG;
  friend class Dot;
  friend class Tree;
  
public:
  BB ();
  lemon::ListDigraph::Node addNode (Inst &);
  lemon::ListDigraph::Arc  addEdge (lemon::ListDigraph::Node &,
				    lemon::ListDigraph::Node &);
  
  lemon::ListDigraph::Node &leader () { return m_leader; };
                      CFG  *call   () { return m_call;   };
  lemon::ListDigraph::Node &ret    () { return m_ret;    };
  
  void leader (lemon::ListDigraph::Node &n) { m_leader = n; };
  void call   (                    CFG  *f) { m_call   = f; };
  void ret    (lemon::ListDigraph::Node &n) { m_ret    = n; };

protected:
  lemon::ListDigraph::Node             m_id;
    std::             string           m_name;
    std::             string           m_label;
  
  lemon::             ListDigraph     *m_graph;
  lemon::ListDigraph::NodeMap<Inst *> *m_instrs;
  lemon::ListDigraph::Node             m_leader;  
                      CFG             *m_call;
  lemon::ListDigraph::Node             m_ret;
};

#endif // _BB_H_
