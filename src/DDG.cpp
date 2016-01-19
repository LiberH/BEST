#include "DDG.hpp"

#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"

#include <vector>
#include <bitset>

using namespace std;
using namespace lemon;

/* Pseudocode:

  for v in V
    IN(v) = {}
    OUT(v) = GEN(v)
  endfor

  worklist := v in V
  while ( worklist != {} )
    pick and remove a node v from worklist
    oldout(v) = OUT(v)
    IN(v) = U (OUT(p)), p in PRED(v)
    OUT(v) = GEN(v) U (IN(v) - KILL(v))
    if oldout(v) != OUT(v) then
      worklist = worklist U SUCC(v)
    endif
  endwhile

*/

DDG::DDG (CFG &cfg)
{
  ostringstream ss_name, ss_label;
  ss_name  << "ddg";
  ss_label << "DDG(" << cfg.m_label << ")";

  m_name  = ss_label.str ();
  m_label = ss_label.str ();
  
  m_kills = new vector< set<Inst *> * > (64, NULL);
  for (int b = 0; b < 64; ++b)
    (*m_kills)[b] = new set<Inst *> ();
  
  ListDigraph::NodeIt k (*cfg.m_graph);
  for (; k != INVALID; ++k)
    {
      BB *bb = (*cfg.m_bbs)[k];
      vector<Inst *>::iterator it = bb -> m_insts -> begin ();
      for (; it != bb -> m_insts -> end (); ++it)
	{
	  Inst *i = *it;
	  bitset<64> bs (i -> m_defs);
	  for (int b = 0; b < 64; ++b)
            if (bs.test (b))
	      (*m_kills)[b] -> insert (i);
	}
    }

  m_gens = new ListDigraph::NodeMap< set<Inst *> * > (*cfg.m_graph, NULL);
  ListDigraph::NodeIt g (*cfg.m_graph);
  for (; g != INVALID; ++g)
    {
      (*m_gens)[g] = new set<Inst *> ();
      BB *bb = (*cfg.m_bbs)[g];
      vector<Inst *>::iterator it = bb -> m_insts -> begin ();
      for (; it != bb -> m_insts -> end (); ++it)
	{
	  Inst *i = *it;
	  bitset<64> bs (i -> m_defs);
	  for (int b = 0; b < 64; ++b)
	    if (bs.test (b))
	      (*m_gens)[g] -> insert (i);
	}
    }
  
  m_ins   = new ListDigraph::NodeMap< set<Inst *> * > (*cfg.m_graph, NULL);
  m_outs  = new ListDigraph::NodeMap< set<Inst *> * > (*cfg.m_graph, NULL);
  m_deps  = new ListDigraph::NodeMap< set<ListDigraph::Node> * > (*cfg.m_graph, NULL);
  
  ListDigraph::NodeIt n (*cfg.m_graph);
  for (; n != INVALID; ++n)
    {
      (*m_ins  )[n] = new set<Inst *> ();
      (*m_outs )[n] = new set<Inst *> ();
      (*m_deps )[n] = new set<ListDigraph::Node>;
    }

  set<Inst *>::iterator it;
  vector<ListDigraph::Node> w;
  w.push_back (cfg.m_entry);
  while (!w.empty ())
    {
      // pick and remove a node from the worklist.
      ListDigraph::Node n = w.front ();
      w.erase (w.begin ());

      // assign 'OUT' to 'OLDOUT'.
      set<Inst *> *oldout = new set<Inst *> ();
      set<Inst *>::iterator it = (*m_outs)[n] -> begin ();
      for (; it != (*m_outs)[n] -> end (); ++it)
	oldout -> insert (*it);

      // compute 'IN'.
      (*m_ins)[n] -> clear ();
      ListDigraph::InArcIt ia (*cfg.m_graph, n);
      for (; ia != INVALID; ++ia)
	{
	  ListDigraph::Node m = cfg.m_graph -> source (ia);
	  set<Inst *>::iterator it = (*m_outs)[m] -> begin ();
	  for (; it != (*m_outs)[m] -> end (); ++it)
	    (*m_ins)[n] -> insert (*it);
	 }

      // compute 'KILL'.
      set<Inst *> *kill = new set<Inst *> ();
      it = (*m_gens)[n] -> begin ();
      for (; it != (*m_gens)[n] -> end (); ++it)
	for (int b = 0; b < 64; ++b)
	  {
	    Inst *i = *it;
	    bitset<64> bs (i -> m_defs);
	    if (bs.test (b))
	      kill -> insert ((*m_kills)[b] -> begin (),
		              (*m_kills)[b] -> end ());
	  }
      
      // compute 'OUT'.
      (*m_outs)[n] -> clear ();
      
      it = (*m_ins)[n] -> begin ();
      for (; it != (*m_ins)[n] -> end (); ++it)
	(*m_outs)[n] -> insert (*it);

      it = kill -> begin ();
      for (; it != kill -> end (); ++it)
	(*m_outs)[n] -> erase (*it);
      
      it = (*m_gens)[n] -> begin ();
      for (; it != (*m_gens)[n] -> end (); ++it)
	(*m_outs)[n] -> insert (*it);

      // check if 'OLDOUT' equals 'OUT'.
      if ((*oldout) != (*(*m_outs)[n]))
	{
	  // add all successors to the worklist.
	  ListDigraph::OutArcIt oa (*cfg.m_graph, n);
	  for (; oa != INVALID; ++oa)
	    w.push_back (cfg.m_graph -> target (oa));
	}
    }
  
  /*
  {
  ListDigraph::NodeIt n (*cfg.m_graph);
  for (; n != INVALID; ++n)
    {
      (*m_deps)[n] = new set<ListDigraph::Node>;
      BB *bb = (*cfg.m_bbs)[n];
      vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();
      for (; inst_it != bb -> m_insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  bitset<64> bs (inst -> m_refs);
	  for (int b = 0; b < 64; ++b)
	    {
	      if (b < 30) continue;
	      if (bs.test (b))
		{
		  set<Inst *>::iterator in_it = (*m_ins)[n] -> begin ();
		  for (; in_it != (*m_ins)[n] -> end (); ++in_it)
		    {
		      Inst *in = *in_it;
		      bitset<64> bs (in -> m_defs);
		      if (bs.test (b))
			{
			  ListDigraph::Node fb = (*cfg.m_nodes)[in -> m_addr];
			  (*m_deps)[n] -> insert (fb);

			  BB *cc = (*cfg.m_bbs)[fb];
			  cout << bb -> m_label << ": ";
			  cout << inst -> m_disass << endl;
			  cout << " r" << b -30;
			  cout << " -> " << cc -> m_label << ": ";
			  cout << in -> m_disass << endl;
			  cout << endl;
			}
		    }
		}
	    }
	  
	}
    }
  }
  */
}

/*
string reg_names[] = {
  "cr"      , "ctr"   , "l1csr0" , "l1csr1" , "l1finv1" , "lr"   , "msr"  , "pc"   ,
  "serial0" , "srr0"  , "srr1"   , "xer"    , "hit"     , "miss" , "fpr0" , "fpr1" ,
  "fpr2"    , "fpr3"  , "fpr4"   , "fpr5"   , "fpr6"    , "fpr7" , "fpr8" , "fpr9" ,
  "fpr10"   , "fpr11" , "fpr12"  , "fpr13"  , "fpr14"  , "fpr15" ,

  "r0"  , "r1"  , "r2"  , "r3"  , "r4"  , "r5"  , "r6"  , "r7"  ,
  "r8"  , "r9"  , "r10" , "r11" , "r12" , "r13" , "r14" , "r15" ,
  "r16" , "r17" , "r18" , "r19" , "r20" , "r21" , "r22" , "r23" ,
  "r24" , "r25" , "r26" , "r27" , "r28" , "r29" , "r30" , "r31" };
*/
