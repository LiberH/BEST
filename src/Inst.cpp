#include "Inst.hpp"

#include "arch.h"
#include "codeReader.h"
#include "staticInfo.h"
#include <sstream>

using namespace std;

/* PRIVATE: */

// static
int Inst::m_id = 0;

/* PUBLIC: */

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
	  insts -> push_back (inst);
	  
	  if (prev)
	    prev -> m_next = inst;
	  prev = inst;
	}
    }

  return insts;
}

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
  m_next    = NULL;
  
  m_branch  = si.is_branch;
  m_unknown = si.is_unknown;
  m_link    = si.do_link;
  m_uncond  = si.is_uncond;
  m_target  = si.target;
}
