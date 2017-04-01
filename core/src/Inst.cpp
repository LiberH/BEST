#include "Inst.hpp"

#include "arch.h"
#include "codeReader.h"
#include "staticInfo.h"
#include <cstdlib>
#include <bitset>
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

  m_num      = id;
  m_name     = ss_name.str ();
  m_label    = ss_label.str ();
  
  m_addr     = si.pc;
  m_disass   = si.mnemo;
  m_function = si.funct;
  m_refs     = si.read_regs;
  m_defs     = si.write_regs;
  m_prev     = NULL;
  m_next     = NULL;
  
  m_branch   = si.is_branch;
  m_test     = si.test;
  m_crfD     = si.crfD;
  m_unknown  = si.is_unknown;
  m_link     = si.do_link;
  m_uncond   = si.is_uncond;
  m_target   = si.target;
  
  m_memory   = si.do_memory;

  // Fix bad branch target in disassembly:
  if (m_branch &&
     !m_uncond &&
     !m_link)
    {
      string s;
      stringstream ss (m_disass);      
      bitset<64> bs (m_refs);
      char delim = (bs[1] ? ' ' : ',');
      getline (ss, m_disass, delim);
      getline (ss, s, delim);
      m_disass += delim;
      
      ostringstream oss;
      long int i = strtol (s.c_str (), NULL, 16);
      oss << hex << i +4;
      m_disass += oss.str ();
    }

  // Fix unspecific depedence to cr:
        int crX_index;
  const int cr__index = 0;
  bitset<64> refs_bs (m_refs);
  bitset<64> defs_bs (m_defs);
  if (refs_bs[cr__index])
    {
      crX_index = m_crfD + 16;
      refs_bs[cr__index] = 0;
      refs_bs[crX_index] = 1;
      m_refs = refs_bs.to_ulong ();
    }
  if (defs_bs[cr__index])
    {
      crX_index = m_crfD + 16;
      defs_bs[cr__index] = 0;
      defs_bs[crX_index] = 1;
      m_defs = defs_bs.to_ulong ();
    }
}

Inst::Inst (const Inst &inst)
{
  m_num      = inst.m_num;
  m_name     = inst.m_name;
  m_label    = inst.m_label;
  
  m_addr     = inst.m_addr;
  m_disass   = inst.m_disass;
  m_function = inst.m_function;
  m_refs     = inst.m_refs;
  m_defs     = inst.m_defs;
  m_prev     = inst.m_prev;
  m_next     = inst.m_next;
  
  m_branch   = inst.m_branch;
  m_test     = inst.m_test;
  m_crfD     = inst.m_crfD;
  m_unknown  = inst.m_unknown;
  m_link     = inst.m_link;
  m_uncond   = inst.m_uncond;
  m_target   = inst.m_target;

  m_memory   = inst.m_memory;
}

// static
vector<Inst *> *
Inst::FromFile (string f, u32 *entry_addr, u32 *exit_addr)
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
  a.getFunctionName ("launchTest", *entry_addr);
  a.getFunctionName ("shouldNotHappen", *exit_addr);
  
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
	  /*
	    size_t pos = info -> mnemo.find("Stall");
	    if (pos != string::npos)
	      break;
	  */
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

      f << hex << inst -> m_addr << ":   "
	<< mnemo << spaces << args
	<< endl;
    }
  
  f.close ();
}

static
string reg_names[] = {
  "cr"      , "ctr"   , "l1csr0" , "l1csr1" , "l1finv1" , "lr"    , "msr"   , "pc"    ,
  "serial0" , "srr0"  , "srr1"   , "xer"    , "hit"     , "miss"  ,

  "fpr0" , "fpr1" , "fpr2"  , "fpr3"  , "fpr4"  , "fpr5"  , "fpr6"  , "fpr7"  ,
  "fpr8" , "fpr9" , "fpr10" , "fpr11" , "fpr12" , "fpr13" , "fpr14" , "fpr15" ,
  
  "r0"   , "r1"   , "r2"    , "r3"    , "r4"    , "r5"    , "r6"    , "r7"    ,
  "r8"   , "r9"   , "r10"   , "r11"   , "r12"   , "r13"   , "r14"   , "r15"   ,
  "r16"  , "r17"  , "r18"   , "r19"   , "r20"   , "r21"   , "r22"   , "r23"   ,
  "r24"  , "r25"  , "r26"   , "r27"   , "r28"   , "r29"   , "r30"   , "r31"   };

// static
int
Inst::CountRegs (vector<Inst *> *insts)
{
  int count = 0;
  u64 refs = 0;
  vector<Inst *>::iterator inst_it = insts -> begin ();  
  for (; inst_it != insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      refs = refs | inst -> m_refs;
    }

  bitset<64> bs (refs);
  for (int b = 0; b < 62; ++b)
    if (bs[b])
      {
	//cout << reg_names[b] << " ";
	count++;
      }

  //cout << endl;
  return count;
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
