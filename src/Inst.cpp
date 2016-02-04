#include "Inst.hpp"

#include "arch.h"
#include "codeReader.h"
#include "staticInfo.h"
#include <sstream>
#include <fstream>
#include <algorithm>

#define C(s) ((char *) (s).c_str ())

using namespace std;
/* PUBLIC */

Inst::Inst (const staticInfo &si)
{
  const int id = m_id++;
  
  ostringstream ss_name, ss_label;
  ss_name  << "i" << id;
  ss_label << hex << si.pc << ": " << si.mnemo;
  
  m_name    = ss_name.str ();
  m_label   = ss_label.str ();
  
  m_addr    = si.pc;
  m_disass  = si.mnemo;
  m_refs    = si.read_regs;
  m_defs    = si.write_regs;
  m_prev    = NULL;
  m_next    = NULL;
  
  m_branch  = si.is_branch;
  m_unknown = si.is_unknown;
  m_link    = si.do_link;
  m_uncond  = si.is_uncond;
  m_target  = si.target;
}

// static
vector<Inst *> *
Inst::FromFile (string f)
{
  arch a;
  codeReader *reader;
  codeSection *section;
  int nbCodeSection;
  u32 addr, end_addr;
  staticInfo *info;
  Inst *inst, *prev;
  vector<Inst *> *insts;
  
  insts = new vector<Inst *> ();
  a.readCodeFile (f.c_str ());
  reader = a.getCodeReader ();
  nbCodeSection = reader -> getNbCodeSection ();
  for (int i = 0; i < nbCodeSection; ++i)
    {
      section = reader -> getCodeSection (i);
      addr = section -> v_addr ();
      end_addr = addr + section -> size ();
      
      prev = NULL;
      while (addr < end_addr)
	{
	  info = a.getInstructionStaticInfo (addr);
	  inst = new Inst (*info);
	  inst -> m_prev = prev;
	  insts -> push_back (inst);
	  
	  if (prev)
	    prev -> m_next = inst;
	  prev = inst;
	}
    }

  return insts;
}

// static
void
Inst::ToFile (string fn, vector<Inst *> *insts)
{
  ofstream f;
  f.open (C(fn));

  sort (insts -> begin (), insts -> end (), byAddr);
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
  
  f.close ();
}

/* PRIVATE */

// static
int Inst::m_id = 0;

// static
bool
Inst::byAddr (const Inst *inst, const Inst *tsni)
{
  return inst -> m_addr < tsni -> m_addr;
}
