#include "BB.hpp"

#include "Inst.hpp"
#include <cstdio>
#include <graphviz/cgraph.h>
#include <sstream>
#include <fstream>
#include <algorithm>

#define C(s) ((char *) (s).c_str ())

using namespace std;

/* PUBLIC */

BB::BB ()
{
  const int id = m_id++;
  
  ostringstream ss_name, ss_label;
  ss_name  << "bb" << id;
  ss_label << "BB" << id;
  
  m_name   = ss_name.str ();
  m_label  = ss_label.str ();

  m_insts = new vector<Inst *> ();
  m_entry  = -1;
}

void
BB::addInst (Inst &i)
{
  i.m_name = m_name + i.m_name;
  m_insts -> push_back (&i);
}

vector<Inst *> *
BB::insts ()
{
  return m_insts;
}

// static
void
BB::FromFile (string f, u32 *entry_addr, u32 *exit_addr, vector<BB *> **bbs,
	      u32 *data_addr, vector<s32> **data,
	      u32 *bss_addr,  vector<s32> **bss )
{
  vector<Inst *> *insts = NULL;
  Inst::FromFile (f, entry_addr, exit_addr, &insts, data_addr, data, bss_addr, bss);
  vector<u32> *leaders = BB::Leaders (*insts);
  
  Inst *inst = insts -> front ();
  insts -> erase (insts -> begin ());
  //leaders -> erase (leaders -> begin ());
  
  BB *bb = new BB ();  
  bb -> addInst (*inst);
  bb -> m_entry = inst -> m_addr;

  *bbs = new vector<BB *> ();  
  vector<Inst *>::iterator inst_it = insts -> begin ();
  for (; inst_it != insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      vector<u32>::iterator addr_it =
	find (leaders -> begin () ,
	      leaders -> end   () ,
	      inst    -> m_addr   );
      
      if (addr_it != leaders -> end ())
	{
	  (*bbs) -> push_back (bb);
	  
	  bb = new BB ();
	  bb -> m_entry = inst -> m_addr;
	}

      bb -> addInst (*inst);
    }

  (*bbs) -> push_back (bb);
}

// static
void
BB::ToFile (string fn, vector<BB *> *bbs)
{
  ofstream f;
  f.open (C(fn));

  sort (bbs -> begin (), bbs -> end (), byAddr);
  vector<BB *>::iterator bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;

      f << "------------------------------- " << bb -> m_label << endl;
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it = insts -> begin ();  
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  
	  string addr, mnemo, spaces, args;
	  stringstream ss (inst -> m_disass);      
	  getline (ss, mnemo, ' ');
	  getline (ss, args,  ' ');
	  spaces = string (8 - mnemo.length (), ' ');
	  
	  f << hex << inst -> m_addr << ":       "
	    << mnemo << spaces << args
	    << endl;
	}
    }
    
  f.close ();
}

// static
void
BB::ToFile (string fn, BB *bb)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(bb -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label",   C(bb -> m_label));  
  agattr (agraph, AGRAPH, "ranksep", "0.2");  
  agattr (agraph, AGNODE, "shape",   "box");  
  agattr (agraph, AGNODE, "width",   "2.0");  
  agattr (agraph, AGNODE, "height",  "0.3");  
  agattr (agraph, AGEDGE, "style",   "solid");

  Agnode_t *agn;
  vector<Agnode_t *> *agnodes = new vector<Agnode_t *>;
  vector<Inst *>::iterator it = bb -> m_insts -> begin();
  
  agn = agnode (agraph, C((*it) -> m_name), TRUE);
  agsafeset (agn, "label", C((*it) -> m_label + "\\l"), "error");
  agnodes -> push_back (agn);
  
  for (++it; it != bb -> m_insts -> end(); ++it)
    {
      agn = agnode (agraph, C((*it) -> m_name), TRUE);
      agedge (agraph, agnodes -> back (), agn, NULL, TRUE);
      agsafeset (agn, "label", C((*it) -> m_label + "\\l"), "error");
      agnodes -> push_back (agn);
    }
  
  agwrite (agraph, f);  
  agclose (agraph);
  fclose (f);
}

// static
vector<u32> *
BB::Leaders (vector<Inst *> &insts)
{
  vector<u32> *leaders = new vector<u32> ();
  vector<Inst *>::iterator inst_it = insts.begin ();
  leaders -> push_back ((*inst_it) -> m_addr);
  for (; inst_it != insts.end (); ++inst_it)
    {
      Inst *i = *inst_it;
      if (i -> m_branch)
	{
	  leaders -> push_back (i -> m_addr +4);
	  leaders -> push_back (i -> m_target);
	}
    }

  vector<u32>::iterator leader_it;
  sort (leaders -> begin (), leaders -> end ());
  leader_it = unique (leaders -> begin (), leaders -> end ());
  leaders -> erase (leader_it, leaders -> end ());

  return leaders;
}

// static
bool
BB::byAddr (const BB *bb, const BB *cc)
{
  return bb -> m_entry < cc -> m_entry;
}

/* PRIVATE */

// static
int BB::m_id = 0;
