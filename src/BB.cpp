#include "BB.hpp"

#include "Inst.hpp"
#include <sstream>
#include <algorithm>

using namespace std;

// static
int BB::m_id = 0;

// static
vector<BB *> *
BB::FromFile (string f)
{
  vector<BB   *> *bbs     = new vector<BB *> ();  
  vector<Inst *> *insts   = Inst::FromFile ( f    );
  vector<u32   > *leaders =   BB::Leaders  (*insts);
  
  Inst *inst = insts -> front ();
  insts   -> erase (insts   -> begin ());
  leaders -> erase (leaders -> begin ());
  
  BB *bb = new BB ();  
  bb -> addInst (*inst);
  bb -> m_entry = inst -> m_addr;

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
	  bbs -> push_back (bb);
	  
	  bb = new BB ();
	  bb -> m_entry = inst -> m_addr;
	}

      bb -> addInst (*inst);
    }

  bbs -> push_back (bb);

  return bbs;
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
