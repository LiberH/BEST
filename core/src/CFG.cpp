#include "CFG.hpp"

#include "arch.h"
#include "Inst.hpp"
#include "BB.hpp"
#include "DFS.hpp"
#include "tinyxml2.h"

#include <cstdio>
#include <graphviz/cgraph.h>
#include <lemon/maps.h>
#include <lemon/dfs.h>
#include <vector>
#include <bitset>
#include <iostream>
#include <iomanip>

#define S(i) static_cast<std::ostringstream &>((std::ostringstream() << std::dec << i)).str()
#define C(s) ((char *) (s).c_str ())

using namespace std;
using namespace lemon;
using namespace tinyxml2;

/* PUBLIC: */

CFG::CFG ()
{
  ostringstream ss_name, ss_label;
  ss_name  << "cfg";
  ss_label << "CFG";

  m_name  = ss_name.str ();
  m_label = ss_label.str ();
  
  m_graph = new ListDigraph ();
  m_nodes = new map<int, ListDigraph::Node> ();
  m_bbs   = new ListDigraph::NodeMap<BB *> (*m_graph, NULL);
  m_preds = new ListDigraph::NodeMap< vector<BB *> *> (*m_graph, NULL);
  m_succs = new ListDigraph::NodeMap< vector<BB *> *> (*m_graph, NULL);

  m_entry = INVALID;
  m_exit  = INVALID;
}

vector<BB *> *
CFG::bbs ()
{
  vector<BB *> *bbs = new vector<BB *> ();
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*m_bbs)[n];
      bbs -> push_back (bb);
    }
  
  return bbs;
}

vector<Inst *> *
CFG::insts ()
{
  vector<Inst *> *insts = new vector<Inst *> ();
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*m_bbs)[n];
      vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();
      for (; inst_it != bb -> m_insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  insts -> push_back (inst);
	}
    }

  return insts;
}

ListDigraph::Node
CFG::addBB (BB &bb)
{
  ListDigraph::Node n = m_graph -> addNode ();
  bb.m_name = m_name + bb.m_name;
  
  (*m_bbs)[n] = &bb;
  vector<Inst *>::iterator inst_it = bb.m_insts -> begin ();
  for (; inst_it != bb.m_insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      (*m_nodes)[inst -> m_addr] = n;
    }
  
  return n;
}

ListDigraph::Arc
CFG::addEdge (BB &bb,
	      BB &cc)
{
  ListDigraph::Node n = (*m_nodes)[bb.m_entry];
  ListDigraph::Node m = (*m_nodes)[cc.m_entry];
  return m_graph -> addArc (n, m);
}

// static
CFG *
CFG::Reverse (const CFG *cfg)
{
  CFG *rcfg = new CFG ();
  rcfg -> m_name  = "REV"  + cfg -> m_name;
  rcfg -> m_label = "Rev(" + cfg -> m_label + ")";
  
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      ListDigraph::Node m = rcfg -> m_graph -> addNode ();
      (*rcfg -> m_bbs)[m] = bb;  
      (*rcfg -> m_nodes)[bb -> m_entry] = m;
    }
  
  ListDigraph::ArcIt a (*cfg -> m_graph);
  for (; a != INVALID; ++a)
    {
      BB *bb = (*cfg -> m_bbs)[cfg -> m_graph -> source (a)];
      BB *cc = (*cfg -> m_bbs)[cfg -> m_graph -> target (a)];
      rcfg -> addEdge (*cc, *bb); // reversed
    }

  BB *bb = (*cfg -> m_bbs)[cfg -> m_entry];
  BB *cc = (*cfg -> m_bbs)[cfg -> m_exit];
  rcfg -> m_entry = (*rcfg -> m_nodes)[cc -> m_entry]; // reversed
  rcfg -> m_exit  = (*rcfg -> m_nodes)[bb -> m_entry]; // id.
  
  return rcfg;
}

// static
CFG *
CFG::FromFile (string f)
{
  u32 entry_addr, exit_addr, data_addr, bss_addr;
  
  CFG *cfg = new CFG ();
  vector<BB *> *bbs  = NULL;
  vector<s32 > *data = NULL;
  vector<s32 > *bss  = NULL;
  BB::FromFile (f, &entry_addr, &exit_addr, &bbs, &data_addr, &data, &bss_addr, &bss);
  cfg -> m_data = data;
  cfg -> m_data_addr = data_addr;
  cfg -> m_bss = bss;
  cfg -> m_bss_addr = bss_addr;
  vector<BB *>::iterator bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;
      cfg -> addBB (*bb);
    }
  
  cfg -> findSuccs (*bbs);
  bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;
      ListDigraph::Node n = (*cfg -> m_nodes)[bb -> m_entry];
      vector<BB *>::iterator succ_it = (*cfg -> m_succs)[n] -> begin ();
      for (; succ_it != (*cfg -> m_succs)[n] -> end (); ++succ_it)
	{
	  BB *succ = *succ_it;
	  cfg -> addEdge (*bb, *succ);
	}
    }

  cfg -> m_entry = (*cfg -> m_nodes)[entry_addr];  
  cfg -> m_exit  = (*cfg -> m_nodes)[exit_addr];
  
  cfg -> blr_patch ();  
  cfg -> deadcode_patch ();
  return cfg;
}

// static
void
CFG::ToFile (string fn, CFG *cfg, int grain)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(cfg -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C(cfg -> m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);

  if (grain == CFG::COARSE_GRAIN)
    {
      ListDigraph::NodeIt n (*graph);
      for (; n != INVALID; ++n)
	{
	  BB *bb = (*cfg -> m_bbs)[n];
	  (*agnodes)[n] = agnode (agraph, C(bb -> m_name), TRUE);
	  agsafeset ((*agnodes)[n], "label", C(bb -> m_label), "error");
	}
  
      ListDigraph::ArcIt a (*graph);
      for (; a != INVALID; ++a)
	{
	  ListDigraph::Node n = graph -> source (a);
	  ListDigraph::Node m = graph -> target (a);
	  agedge (agraph, (*agnodes)[n], (*agnodes)[m], NULL, TRUE);
	}
    }
  else
    {
      vector<Inst *> *insts = cfg -> insts ();
      sort (insts -> begin (), insts -> end (), Inst::byAddr);
      vector<Inst *>::iterator inst_it = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  ostringstream oss;
	  oss.str (""); oss << hex << inst -> m_addr;

	  Agnode_t *src = agnode (agraph, C(inst -> m_name), TRUE);
	  agsafeset (src, "label", C(oss.str ()), "error");
	  
	  if(!inst -> m_branch)
	    {
	      string name = (inst -> m_next != NULL ? inst -> m_next -> m_name : "NULL"); // TOODO: fix
	      Agnode_t *nxt = agnode (agraph, C(name), TRUE);
	      agedge (agraph, src, nxt, NULL, TRUE);
	    }
	  else if (inst -> m_uncond)
	    {
	      Inst *target = NULL;
	      vector<Inst *> *targets = cfg -> insts ();
	      vector<Inst *>::iterator target_it = targets -> begin ();
	      for (; target_it != targets -> end (); ++target_it)
		{
		  target = *target_it;
		  if (target -> m_addr == inst -> m_target)
		    break;
		}

	      Agnode_t *trg = agnode (agraph, C(target -> m_name), TRUE);
	      agedge (agraph, src, trg, NULL, TRUE);
	    }
	  else
	    {
	      Agnode_t *nxt = agnode (agraph, C(inst -> m_next -> m_name), TRUE);
	      agedge (agraph, src, nxt, NULL, TRUE);
	      
	      Inst *target = NULL;
	      vector<Inst *> *targets = cfg -> insts ();
	      vector<Inst *>::iterator target_it = targets -> begin ();
	      for (; target_it != targets -> end (); ++target_it)
		{
		  target = *target_it;
		  if (target -> m_addr == inst -> m_target)
		    break;
		}

	      Agnode_t *trg = agnode (agraph, C(target -> m_name), TRUE);
	      agedge (agraph, src, trg, NULL, TRUE);
	    }
	}
    }
  
  agwrite (agraph, f);
  agclose (agraph);
  fclose (f);
}

typedef
struct {
  char *name;
  char *value;
} attr_t;

XMLElement *
newElementWrapper (XMLDocument *doc,
		   const char *name,
		   const char *text,
		   attr_t *attrs,
		   XMLElement **childs)
{
  XMLElement *elm;
  
  elm = doc -> NewElement (name);
  if (text != NULL)
    elm -> SetText (text);
  
  if (attrs != NULL)
    for (; attrs -> name != NULL; ++attrs)
      elm -> SetAttribute (attrs -> name, attrs -> value);

  if (childs != NULL)
    for (; (*childs) != NULL; ++childs)
      elm -> InsertEndChild (*childs);
  
  return elm;
}

XMLElement *
CFG::fall_through (XMLDocument *doc, Inst * src_inst, Inst *trg_inst, bool in_slice)
{
  XMLElement *tr;
  string src, trg, grd, upd;
  ostringstream oss, oss_grd, oss_upd;
  
  oss.str (""); oss << "id" << hex << src_inst -> m_addr; src = oss.str ();
  oss.str (""); oss << "id" << hex << trg_inst -> m_addr; trg = oss.str ();
  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t tr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};

  oss_grd << "EUs_ExecuteNext(" << dec << src_inst -> m_num  << ")";
  if (in_slice) { oss_upd << "execute_" << hex << src_inst -> m_addr << "()"; }
   
  grd = oss_grd.str ();
  upd = oss_upd.str ();
  
  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL          , tr_src_attrs , NULL);
  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL          , tr_trg_attrs , NULL);
  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd)        , tr_grd_attrs , NULL);
  XMLElement *tr_syn = newElementWrapper (doc, "label"  , "EUs_doStep?" , tr_syn_attrs , NULL);
  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd)        , tr_upd_attrs , NULL);

  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_syn, tr_upd, NULL};
  tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);

  return tr;
}

XMLElement *
CFG::fall_into (XMLDocument *doc, Inst * src_inst, Inst *trg_inst, bool in_slice, bool taken)
{
  XMLElement *tr;
  string src, trg, grd, upd;
  ostringstream oss, oss_grd, oss_upd;
  
  oss.str (""); oss << "id" << hex << src_inst -> m_addr; src = oss.str ();
  oss.str (""); oss << "id" << hex << trg_inst -> m_addr; trg = oss.str ();
  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t tr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};

  oss_grd << (taken ? "" : "!");
  switch (src_inst -> m_test)
    {
    case  0: oss_grd << "true"; break;

    case  1: oss_grd << "lt(cr" << src_inst -> m_crfD << ")"; break;
    case  2: oss_grd << "gt(cr" << src_inst -> m_crfD << ")"; break;
    case  3: oss_grd << "eq(cr" << src_inst -> m_crfD << ")"; break;
    case  4: oss_grd << "so(cr" << src_inst -> m_crfD << ")"; break;

    case  5: oss_grd << "ge(cr" << src_inst -> m_crfD << ")"; break;
    case  6: oss_grd << "le(cr" << src_inst -> m_crfD << ")"; break;
    case  7: oss_grd << "ne(cr" << src_inst -> m_crfD << ")"; break;

    case  9: oss_grd <<  "z()"; break;
    case 10: oss_grd << "nz()"; break;

    default: oss_grd << "####"; break;
    }

  oss_grd << " &&" << endl << "EUs_ExecuteNext(" << dec << src_inst -> m_num  <<  ")";
  if (in_slice) { oss_upd << "execute_" << hex << src_inst -> m_addr << "()"; }
  
  size_t bclr_pos = src_inst -> m_disass.find ("bclr- ");
  if (bclr_pos != string::npos
  &&  taken)
    oss_grd << " &&" << endl << "_Stack_TopIs(" << dec << trg_inst -> m_num << ")";

  grd = oss_grd.str ();
  upd = oss_upd.str ();

  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL          , tr_src_attrs , NULL);
  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL          , tr_trg_attrs , NULL);
  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd)        , tr_grd_attrs , NULL);
  XMLElement *tr_syn = newElementWrapper (doc, "label"  , "EUs_doStep?" , tr_syn_attrs , NULL);
  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd)        , tr_upd_attrs , NULL);
  
  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_syn, tr_upd, NULL};
  tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
  
  return tr;
}

static
string reg_names[64] = {
  /*  0 */ "cr"   , /*  1 */ "ctr" , /*  2 */ "XX2" , /*  3 */ "XX3" , /*  4 */ "XX4" , /*  5 */ "lr"  , /*  6 */ "XX6" , /*  7 */ "pc"  ,
  /*  8 */ "XX8"  , /*  9 */ "XX9" , /* 10 */ "X10" , /* 11 */ "xer" , /* 12 */ "X12" , /* 13 */ "X13" , /* 14 */ "X14" , /* 15 */ "X15" ,
  /* 16 */ "cr0"  , /* 17 */ "cr1" , /* 18 */ "cr2" , /* 19 */ "cr3" , /* 20 */ "cr4" , /* 21 */ "cr5" , /* 22 */ "cr6" , /* 23 */ "cr7" ,
  /* 24 */  "X24" , /* 25 */ "X25" , /* 26 */ "X26" , /* 27 */ "X27" , /* 28 */ "X28" , /* 29 */ "X29" ,
  
  /* 30 */ "r0"  , /* 31 */ "r1"  , /* 32 */ "r2"  , /* 33 */ "r3"  , /* 34 */ "r4"  , /* 35 */ "r5"  , /* 36 */ "r6"  , /* 37 */ "r7"  ,
  /* 38 */ "r8"  , /* 39 */ "r9"  , /* 40 */ "r10" , /* 41 */ "r11" , /* 42 */ "r12" , /* 43 */ "r13" , /* 44 */ "r14" , /* 45 */ "r15" ,
  /* 46 */ "r16" , /* 47 */ "r17" , /* 48 */ "r18" , /* 49 */ "r19" , /* 50 */ "r20" , /* 51 */ "r21" , /* 52 */ "r22" , /* 53 */ "r23" ,
  /* 54 */ "r24" , /* 55 */ "r25" , /* 56 */ "r26" , /* 57 */ "r27" , /* 58 */ "r28" , /* 59 */ "r29" , /* 60 */ "r30" , /* 61 */ "r31" ,
  /* 62 */ "X62" , /* 63 */ "X63"
};

struct pos {int x; int y;};

// static
void
CFG::ToUPPAAL (string fn, string template_fn, CFG *cfg, vector<Inst *> *slice)
{
  {
    string pos_fn;
    pos_fn = fn.substr (0, fn.find_last_of ("-")) + ".dot";

    FILE *f = fopen (C(pos_fn), "r");
    if (f == NULL) {
      cerr << "CFG::ToUPPAAL: " << pos_fn << " not found." << endl;
      return;
    }
    
    Agraph_t* g = agread (f, NULL);
    Agnode_t *n = agfstnode(g);
    for (; n != NULL; n = agnxtnode (g, n))
      {
	char *label = agget (n, "label");
	char *pos   = agget (n, "pos");
	pos = (pos ? pos : (char *) "null");

	string x, y;
	stringstream ss (pos);
	getline (ss, x, ',');
	getline (ss, y, ',');

	u32 addr;
	stringstream ss_label (label);
	stringstream ss_x (x);
	stringstream ss_y (y);
	ss_label >> hex >> addr;

	Inst *inst = NULL;
	vector<Inst *> *insts = cfg -> insts ();
	vector<Inst *>::iterator inst_it = insts -> begin ();
	for (; inst_it != insts -> end (); ++inst_it)
	  {
	    inst = *inst_it;
	    if (inst -> m_addr == addr)
	      break;
	  }

	ss_x >> dec >> inst -> m_x;
	ss_y >> dec >> inst -> m_y;
	inst -> m_x *=  5;
	inst -> m_y *= -1;
      }
    
    agclose (g);
    fclose (f);
  }

  //////////////

  ostringstream oss;
  XMLDocument *doc = new XMLDocument ();
  doc -> LoadFile (C(template_fn));

  // <nta> 
  XMLElement *nta = doc -> FirstChildElement ("nta");

  vector<Inst *> *insts      = cfg -> insts ();
  vector<s32   > *data       = cfg -> m_data;
  u32             data_addr  = cfg -> m_data_addr;
  vector<s32   > *bss        = cfg -> m_bss;
  u32             bss_addr   = cfg -> m_bss_addr;
  
  u64 regs = 0;
  ostringstream semantics_oss;
  int slice_size = slice -> size ();
  sort (slice -> begin (), slice -> end (), Inst::byAddr);
  for (int i = 0; i < slice_size; ++i)
    {
      Inst *inst = (*slice)[i];
      regs |= inst -> m_refs | inst -> m_defs;
      /* TODO: should not add defs regs when slicing
      refs |= inst -> m_refs;
      defs |= inst -> m_defs;
      diff = refs ^ defs;
      // `diff' is the list of set but not used registers
      //   (as there shall not be use but not set registers).
      // wrt. that these reisters could be declared as `const' in the model.
       */
      
      /////
      
      semantics_oss << "void execute_" << hex << inst -> m_addr << "() {"
		    << " " << inst -> m_function << "; "
		    << "}" << endl;
    }

  int n_rets = 0;
  int insts_size = insts -> size ();
  vector<int> indRets (insts_size);
  sort (insts -> begin (), insts -> end (), Inst::byAddr);
  for (int i = 0; i < insts_size; ++i)
    {
      Inst *inst = (*insts)[i];
      
      indRets[i] = -1;
      size_t bclr_pos = inst -> m_disass.find ("bclr- ");
      if (bclr_pos != string::npos)
	indRets[i] = n_rets++;
    }
  
  int n_insts = insts -> size ();
  ostringstream inst_max_oss;
  inst_max_oss << " " << dec << n_insts +32 << ";";
  string inst_max_str = inst_max_oss.str ();

  int n_data = data -> size () * 4;
  ostringstream data_max_oss;
  data_max_oss << " " << dec << ((n_data == 0) ? 1 : n_data) << ";";
  string data_max_str = data_max_oss.str ();

  ostringstream data_addr_oss;
  data_addr_oss << " " << dec << ((n_data == 0) ? 0 : data_addr) << ";";
  string data_addr_str = data_addr_oss.str ();

  int n_bss = bss -> size () * 4;
  ostringstream bss_max_oss;
  bss_max_oss << " " << dec << ((n_bss == 0) ? 1 : n_bss) << ";";
  string bss_max_str = bss_max_oss.str ();

  ostringstream bss_addr_oss;
  bss_addr_oss << " " << dec << ((n_bss == 0) ? 0 : bss_addr) << ";";
  string bss_addr_str = bss_addr_oss.str ();

  int ri  = 1;
  int cr_count = 1, cr_num;
  int indCR[8] = { 0, -1, -1, -1, -1, -1, -1, -1 };
  ostringstream crs_oss;
  ostringstream gprs_oss;
  bitset<64> bs (regs);
  for (int b = 0; b < 64; ++b)
    if (bs[b])
      switch (b)
	{
	case  0: /* "cr"  */ break; /* replaced by cr0 to cr7 */
	case  1: /* "ctr" */ break; /* already in template    */
	case  5: /* "lr"  */ break; /* not used               */
	case  7: /* "pc"  */ break; /* not used               */
	case 11: /* "xer" */ break; /* already in template    */
	case 16: /* "cr0" */ break; /* already in template    */
	case 17: /* "cr1" */
	case 18: /* "cr2" */
	case 19: /* "cr3" */
	case 20: /* "cr4" */
	case 21: /* "cr5" */
	case 22: /* "cr6" */
	case 23: /* "cr7" */
	  cr_num = b -16;
	  indCR[cr_num] = cr_count++;
	  crs_oss << "const uint3_t " << reg_names[b] << " = " << dec << cr_num << ";" << endl;
	  break;
	  
	case 30: /* "r0"   */ break; /* already in template    */
	case 31: /* "r1"   */
	case 32: /* "r2"   */
	case 33: /* "r3"   */
	case 34: /* "r4"   */
	case 35: /* "r5"   */
	case 36: /* "r6"   */
	case 37: /* "r7"   */
	case 38: /* "r8"   */
	case 39: /* "r9"   */
	case 40: /* "r10"  */
	case 41: /* "r11"  */
	case 42: /* "r12"  */
	case 43: /* "r13"  */
	case 44: /* "r14"  */
	case 45: /* "r15"  */
	case 46: /* "r16"  */
	case 47: /* "r17"  */
	case 48: /* "r18"  */
	case 49: /* "r19"  */
	case 50: /* "r20"  */
	case 51: /* "r21"  */
	case 52: /* "r22"  */
	case 53: /* "r23"  */
	case 54: /* "r24"  */
	case 55: /* "r25"  */
	case 56: /* "r26"  */
	case 57: /* "r27"  */
	case 58: /* "r28"  */
	case 59: /* "r29"  */
	case 60: /* "r30"  */
	case 61: /* "r31"  */
	  gprs_oss << "const uint5_t " << reg_names[b] << " = " << ri++ << ";" << endl;	
	  break;
	  
	default:
	  //cerr << "Bad register: " << reg_names[b] << endl;
	  break;
      }
  string crs_str = crs_oss.str ();
  string gprs_str = gprs_oss.str ();

  ostringstream indRets_oss;
  indRets_oss << " {";
  for (int indRets_i = 0; indRets_i < insts_size -1; ++indRets_i)
    {
      if (indRets_i % 10 == 0)
	indRets_oss << endl << "  ";
      indRets_oss << dec << indRets[indRets_i] << ", ";
    }
  indRets_oss << dec << indRets[insts_size -1] << endl << "};";
  string indRets_str = indRets_oss.str ();
  
  ostringstream rets_oss;
  rets_oss << " {";
  for (int rets_i = 0; rets_i < n_rets; ++rets_i)
    rets_oss << " " << -1 << ",";
  rets_oss.seekp (-1, rets_oss.cur);
  rets_oss << " };";
  string rets_str = rets_oss.str ();

  int ind_i = 1;
  ostringstream indCR_oss;
  indCR_oss << " { 0, ";
  for (; ind_i < 7; ++ind_i)
    indCR_oss << dec << indCR[ind_i] << ", ";
  indCR_oss << dec << indCR[7] << " };";
  string indCR_str = indCR_oss.str ();

  ostringstream rets_max_oss;
  rets_max_oss << " " << dec << n_rets << ";";
  string rets_max_str = rets_max_oss.str ();
  
  int n_cr = cr_count;
  ostringstream cr_max_oss;
  cr_max_oss << " " << dec << n_cr << ";";
  string cr_max_str = cr_max_oss.str ();

  int n_gprs = ri;
  ostringstream gprs_max_oss;
  gprs_max_oss << " " << dec << ((n_gprs == 0) ? 1 : n_gprs) << ";";
  string gprs_max_str = gprs_max_oss.str ();
  
  ostringstream insts_oss;
  insts_oss << " {";
  int last_addr = -1;
  vector<BB *> *bbs = cfg -> bbs ();
  sort (bbs -> begin (), bbs -> end (), BB::byAddr);
  vector<BB *>::iterator bb_it = bbs -> begin ();  
  for (; bb_it != bbs -> end (); ++bb_it)
    //ListDigraph::NodeIt o (*cfg -> m_graph);
    //for (; o != INVALID; ++o)
    {
      BB *bb = *bb_it;

      insts_oss << endl;
      insts_oss << endl << "  /* " << bb -> m_label << " */";

      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  string disass, spaces;
	  ostringstream o;

	  if (next(inst_it) == insts -> end ())
	    last_addr = inst -> m_addr;
	  
	  int target_num = -1;
	  vector<Inst *> *targets = cfg -> insts ();
	  vector<Inst *>::iterator target_it = targets -> begin ();
	  for (; target_it != targets -> end (); ++target_it)
	    {
	      Inst *target = *target_it;
	      if (inst -> m_branch
	       && target -> m_addr == inst -> m_target)
		target_num = target -> m_num; 
	    }

	  o.str ("");
	  o << hex << inst -> m_addr << ": " << inst -> m_disass;
	  disass = o.str ();
	  spaces = string (30 - disass.length (), ' ');
	  
	  insts_oss << endl;
	  insts_oss << "  /*  " << disass << spaces << " - " << dec << inst -> m_num << " */ { ";
	  insts_oss << dec << inst -> m_addr                    << ", ";
	  insts_oss << 1                                        << ", "; // latency;
	  insts_oss << (inst -> m_branch ? "true, " : "false,") << " ";

	  size_t bclr_pos = inst -> m_disass.find ("bclr- ");
	  if (inst -> m_branch
          &&  bclr_pos == string::npos)
	    {
	      o.str ("");
	      o << dec << target_num;
	      spaces = string (9 - o.str ().length (), ' ');
	      insts_oss << spaces << dec << target_num;
	    }
	  else insts_oss << "_INST_MAX";
	  insts_oss << ", ";
	  insts_oss << (inst -> m_memory ? "true, " : "false,") << " "; // does_mem_access
	  insts_oss << "{ ";

	  {
	    int i = 64;
	    ostringstream o;
	    string byte, spaces;
	    bool first = true;
	    do
	      {
		i -= 8;
		
		o.str ("");
		o << ((inst -> m_refs >> i) & 0xff);
		byte = o.str ();
		
		spaces = string (3 - byte.length (), ' ');
		insts_oss << (first ? "" : ", ") << spaces << byte;
		first = false;
	      }
	    while (i);
	  }
	  
	  insts_oss << " }, { ";

	  {
	    int i = 64;
	    ostringstream o;
	    string byte, spaces;
	    bool first = true;
	    do
	      {
		i -= 8;
		
		o.str ("");
		o << ((inst -> m_defs >> i) & 0xff);
		byte = o.str ();
		
		spaces = string (3 - byte.length (), ' ');
		insts_oss << (first ? "" : ", ") << spaces << byte;
		first = false;
	      }
	    while (i);
	  }
	  
	  insts_oss << " } },";
	}
    }

  insts_oss << endl;
  insts_oss << endl << "  /* Nops */";
  for (int i = 0; i < 32; ++i)
    {
      insts_oss << endl << "  /*  xxxx: ---                      - " << dec << n_insts +i << " */";
      insts_oss << " { " << dec << (last_addr + (i +1)*4) << ", 1, false, _INST_MAX, false,";
      insts_oss << " {   0,   0,   0,   0,   0,   0,   0,   0 },";
      insts_oss << " {   0,   0,   0,   0,   0,   0,   0,   0 } },";
    }
  
  insts_oss.seekp (-1, insts_oss.cur);
  insts_oss << endl << "};";
  string insts_str = insts_oss.str ();

  int i = 0, j = 0;
  ostringstream data_oss;
  data_oss << " {" << endl << "  ";
  if (n_data == 0) data_oss << "0 " << endl;
  vector<s32>::iterator byte_it = data -> begin ();
  for (; byte_it != data -> end (); ++byte_it, ++i, j += 4)
    {
      ostringstream o;
      string oct0_str, oct1_str, oct2_str, oct3_str;
      string oct0_spc, oct1_spc, oct2_spc, oct3_spc;
      s32 byte = *byte_it;
      
      signed char oct0 = (byte >>  0) & 0xff;
      o.str (""); o << dec << +oct0;
      oct0_str = o.str ();
      oct0_spc = string (4 - oct0_str.length (), ' ');
      
      signed char oct1 = (byte >>  8) & 0xff;
      o.str (""); o << dec << +oct1;
      oct1_str = o.str ();
      oct1_spc = string (4 - oct1_str.length (), ' ');
      
      signed char oct2 = (byte >> 16) & 0xff;
      o.str (""); o << dec << +oct2;
      oct2_str = o.str ();
      oct2_spc = string (4 - oct2_str.length (), ' ');
      
      signed char oct3 = (byte >> 24) & 0xff;
      o.str (""); o << dec << +oct3;
      oct3_str = o.str ();
      oct3_spc = string (4 - oct3_str.length (), ' ');
      
      if (i == 4) { i = 0; data_oss << endl << "  "; }
      data_oss << "/* " << dec << j    << " */ "
	       << oct3_spc << oct3_str << ", "
	       << oct2_spc << oct2_str << ", "
	       << oct1_spc << oct1_str << ", "
	       << oct0_spc << oct0_str << ", ";
    }
  data_oss.seekp (-2, data_oss.cur);
  data_oss << endl << "};";
  string data_str = data_oss.str ();

  i = 0;
  ostringstream bss_oss;
  bss_oss << " {" << endl << "  ";
  if (bss -> size () == 0) bss_oss << "0       " << endl;
  byte_it = bss -> begin ();
  for (; byte_it != bss -> end (); ++byte_it, ++i)
    {
      if (i == 4) { i = 0; bss_oss << endl << "  "; }
      bss_oss << "   0,    0,    0,    0, /* */ ";
    }
  bss_oss.seekp (-8, bss_oss.cur);
  bss_oss << endl << "};      ";
  string bss_str = bss_oss.str ();

  /////
  
  // <declaration>
  
  XMLElement *nta_decl = nta -> FirstChildElement ("declaration");
  string nta_decl_txt = nta_decl -> GetText ();

  string::size_type index;
  string pattern_inst_max  = " /* REPLACE_WITH_INST_MAX */ 1;";
  string pattern_data_max  = " /* REPLACE_WITH_DATA_MAX */ 1;";
  string pattern_bss_max   = " /* REPLACE_WITH_BSS_MAX */ 1;";
  string pattern_rets_max  = " /* REPLACE_WITH_RETS_MAX */ 1;";
  string pattern_rets      = " /* REPLACE_WITH_RETS */ { -1 };";
  string pattern_indrets   = " /* REPLACE_WITH_INDRETS */ { -1 };";
  string pattern_indcr     = " /* REPLACE_WITH_INDCR */ { -1, -1, -1, -1, -1, -1, -1, -1 };";
  string pattern_cr_max    = " /* REPLACE_WITH_CR_MAX */ 1;";
  string pattern_gprs_max  = " /* REPLACE_WITH_GPRS_MAX */ 1;";
  string pattern_crs       = " /* REPLACE_WITH_CRS */";
  string pattern_gprs      = " /* REPLACE_WITH_GPRS */";
  string pattern_insts     = " /* REPLACE_WITH_INSTS */ { _EMPTY_INST };";
  string pattern_data_addr = " /* REPLACE_WITH_DATA_ADDR */ 0;";
  string pattern_data      = " /* REPLACE_WITH_DATA */ { 0 };";
  string pattern_bss_addr  = " /* REPLACE_WITH_BSS_ADDR */ 0;";
  string pattern_bss       = " /* REPLACE_WITH_BSS */ { 0 };";

  index = 0;
  index = nta_decl_txt.find (pattern_inst_max, index);
  nta_decl_txt.replace (index, pattern_inst_max.length (), inst_max_str);

  index = 0;
  index = nta_decl_txt.find (pattern_data_max, index);
  nta_decl_txt.replace (index, pattern_data_max.length (), data_max_str);

  index = 0;
  index = nta_decl_txt.find (pattern_bss_max, index);
  nta_decl_txt.replace (index, pattern_bss_max.length (), bss_max_str);
  
  index = 0;
  index = nta_decl_txt.find (pattern_rets_max, index);
  nta_decl_txt.replace (index, pattern_rets_max.length (), rets_max_str);
    
  index = 0;
  index = nta_decl_txt.find (pattern_rets, index);
  nta_decl_txt.replace (index, pattern_rets.length (), rets_str);

  index = 0;
  index = nta_decl_txt.find (pattern_indrets, index);
  nta_decl_txt.replace (index, pattern_indrets.length (), indRets_str);

  index = 0;
  index = nta_decl_txt.find (pattern_indcr, index);
  nta_decl_txt.replace (index, pattern_indcr.length (), indCR_str);

  index = 0;
  index = nta_decl_txt.find (pattern_cr_max, index);
  nta_decl_txt.replace (index, pattern_cr_max.length (), cr_max_str);

  index = 0;
  index = nta_decl_txt.find (pattern_gprs_max, index);
  nta_decl_txt.replace (index, pattern_gprs_max.length (), gprs_max_str);
  
  index = 0;
  index = nta_decl_txt.find (pattern_crs, index);
  nta_decl_txt.replace (index, pattern_crs.length (), crs_str);

  index = 0;
  index = nta_decl_txt.find (pattern_gprs, index);
  nta_decl_txt.replace (index, pattern_gprs.length (), gprs_str);

  index = 0;
  index = nta_decl_txt.find (pattern_insts, index);
  nta_decl_txt.replace (index, pattern_insts.length (), insts_str);

  index = 0;
  index = nta_decl_txt.find (pattern_data_addr, index);
  nta_decl_txt.replace (index, pattern_data_addr.length (), data_addr_str);

  index = 0;
  index = nta_decl_txt.find (pattern_data, index);
  nta_decl_txt.replace (index, pattern_data.length (), data_str);

  index = 0;
  index = nta_decl_txt.find (pattern_bss_addr, index);
  nta_decl_txt.replace (index, pattern_bss_addr.length (), bss_addr_str);

  index = 0;
  index = nta_decl_txt.find (pattern_bss, index);
  nta_decl_txt.replace (index, pattern_bss.length (), bss_str);

  /////
  
  oss.str ("");
  oss << nta_decl_txt << endl;
  oss << semantics_oss.str ();
  nta_decl -> SetText (C(oss.str ()));
  // </declaration>
  
  //////

  // <tempalte>
  XMLElement *tmplt = nta -> FirstChildElement ("template");

  /*****************/
  /*****************/
  /*****************/
  
  ////////////////////////////
  // <location id="idinit"> //
  ////////////////////////////
  
               Inst *entry   = (*cfg -> m_bbs  )[cfg   -> m_entry] -> m_insts -> front ();
  ListDigraph::Node  entry_n = (*cfg -> m_nodes)[entry -> m_addr ];
  attr_t entry_attrs[] = {{"id" , "idinit"            },
			  {"x"  , C(S(entry -> m_x))},
			  {"y"  , C(S(entry -> m_y))},
			  {NULL , NULL                }};
  XMLElement *urg = newElementWrapper (doc, "urgent", NULL, NULL, NULL);
  XMLElement *loc_init_childs[] = {urg, NULL};
  XMLElement *loc_init = newElementWrapper (doc, "location", NULL, entry_attrs, loc_init_childs);
  tmplt -> InsertEndChild (loc_init);

  ////////////////////////////
  // <location id="idexit"> //
  ////////////////////////////

               Inst *exit   = (*cfg -> m_bbs  )[cfg  -> m_exit] -> m_insts -> back ();
  ListDigraph::Node  exit_n = (*cfg -> m_nodes)[exit -> m_addr];
  attr_t exit_attrs[] = {{"id" , "idexit"           },
			 {"x"  , C(S(exit -> m_x))},
			 {"y"  , C(S(exit -> m_y))},
			 {NULL , NULL               }};
  XMLElement *loc_exit = newElementWrapper (doc, "location", NULL, exit_attrs, NULL);
  tmplt -> InsertEndChild (loc_exit);

  ///////////////////////////
  // <location id="id..."> //
  ///////////////////////////

  {
  vector<BB *> *bbs = cfg -> bbs ();
  sort (bbs -> begin (), bbs -> end (), BB::byAddr);
  vector<BB *>::iterator bb_it = bbs -> begin ();
  for (; bb_it != bbs -> end (); ++bb_it)
    {
      BB *bb = *bb_it;
      Inst *bb_entry = bb -> m_insts -> front ();

      vector<Inst *> *insts = bb -> m_insts;
      sort (insts -> begin (), insts -> end (), Inst::byAddr);
      vector<Inst *>::iterator inst_it = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  
	  XMLElement *loc_name = NULL;
	  if (inst == bb_entry)
	    {
	      oss.str (""); oss << bb -> m_label;
	      loc_name = newElementWrapper (doc, "name", C(oss.str ()), NULL, NULL);
	    }

	  oss.str (""); oss << "id" << hex << inst -> m_addr;
	  attr_t attrs[] = {{"id" , C(oss.str ())    },
			    {"x"  , C(S(inst -> m_x))},
			    {"y"  , C(S(inst -> m_y))},
			    {NULL , NULL             }};
	  XMLElement *loc_childs[] = {loc_name, NULL};
	  XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, loc_childs);
	  tmplt -> InsertEndChild (loc);
	}
    }
  }
  
  /////////////////////////
  // <init ref="idinit"> //
  /////////////////////////
  
  attr_t init_attrs[] = {{"ref", "idinit"}, {NULL, NULL}};
  XMLElement *init = newElementWrapper (doc, "init", NULL, init_attrs, NULL);
  tmplt -> InsertEndChild (init);

  ///////////////////////
  // First transition: //
  ///////////////////////

  oss.str (""); oss << "id" << hex << entry -> m_addr;  
  attr_t ftr_src_attrs[] = {{"ref"  , "idinit"         } , {NULL , NULL}};
  attr_t ftr_trg_attrs[] = {{"ref"  , C(oss.str ())    } , {NULL , NULL}};
  attr_t ftr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t ftr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
  
  oss.str (""); oss << "InCU_PC_Set(" << dec << entry -> m_num << "),\n_clock = 0";
  XMLElement *ftr_src = newElementWrapper (doc, "source" , NULL             , ftr_src_attrs , NULL);
  XMLElement *ftr_trg = newElementWrapper (doc, "target" , NULL             , ftr_trg_attrs , NULL);
  XMLElement *ftr_syn = newElementWrapper (doc, "label"  , "_doInitialize?" , ftr_syn_attrs , NULL);
  XMLElement *ftr_upd = newElementWrapper (doc, "label"  , C(oss.str ())    , ftr_upd_attrs , NULL);

  XMLElement *ftr_childs[] = {ftr_src, ftr_trg, ftr_syn, ftr_upd, NULL};
  XMLElement *ftr = newElementWrapper (doc, "transition", NULL, NULL, ftr_childs);
  tmplt -> InsertEndChild (ftr);

  //////////////////////
  // Last transition: //
  //////////////////////

  oss.str (""); oss << "id" << hex << exit -> m_addr;
  attr_t ltr_src_attrs[] = {{"ref"  , C(oss.str ())    } , {NULL , NULL}};
  attr_t ltr_trg_attrs[] = {{"ref"  , "idexit"         } , {NULL , NULL}};
  attr_t ltr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t ltr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t ltr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};

  string grd, upd;
  oss.str (""); oss << "EUs_ExecuteNext(" << dec << exit -> m_num <<   ")"; grd = oss.str ();
  oss.str (""); oss << "execute_"         << hex << exit -> m_addr << "()," << endl
		    << "_mustTerminate = true"; upd = oss.str ();
  XMLElement *ltr_src = newElementWrapper (doc, "source" , NULL          , ltr_src_attrs , NULL);
  XMLElement *ltr_trg = newElementWrapper (doc, "target" , NULL          , ltr_trg_attrs , NULL);
  XMLElement *ltr_grd = newElementWrapper (doc, "label"  , C(grd)        , ltr_grd_attrs , NULL);
  XMLElement *ltr_syn = newElementWrapper (doc, "label"  , "EUs_doStep?" , ltr_syn_attrs , NULL);
  XMLElement *ltr_upd = newElementWrapper (doc, "label"  , C(upd)        , ltr_upd_attrs , NULL);

  XMLElement *ltr_childs[] = {ltr_src, ltr_trg, ltr_grd, ltr_syn, ltr_upd, NULL};
  XMLElement *ltr = newElementWrapper (doc, "transition", NULL, NULL, ltr_childs);
  tmplt -> InsertEndChild (ltr);

  ////////////////////////
  // Other transitions: //
  ////////////////////////

  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      bool            in_slice  = false;
      BB             *bb        = (*cfg -> m_bbs)[n];
      vector<Inst *> *insts     = bb -> insts ();
      Inst           *last_inst = insts -> back ();

      /* Check if last inst. of BB is in the slice: */
      vector<Inst *>::iterator slice_it = find (slice -> begin (), slice -> end (), last_inst);
      if (slice_it != slice -> end ())
	in_slice = true;

      /* Building intra-BB transitions: */
      vector<Inst *>::iterator inst_it = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  XMLElement *tr;
	  bool        in_slice =  false;
	  Inst        *inst     = *inst_it;

	  /* Don't take account of last inst. of BB: */
	  if (inst -> m_addr != last_inst -> m_addr)
	    {	      
	      /* Check if this inst. is in the slice: */
	      vector<Inst *>::iterator slice_it = find (slice -> begin (), slice -> end (), inst);
	      if (slice_it != slice -> end ())
		in_slice = true;

	      tr = fall_through (doc, inst, inst -> m_next, in_slice);
	      tmplt -> InsertEndChild (tr);
	    }
	}
      
      /* Building inter-BB transitions (last inst. of BB as source inst.): */
      vector<BB *> *succs = (*cfg -> m_succs)[n];
      vector<BB *>::iterator succ_it = succs -> begin ();
      for (; succ_it != succs -> end (); ++succ_it)
	{
	  XMLElement     *tr;
	  bool            taken      =  true;
	  BB             *succ       = *succ_it;
	  vector<Inst *> *succ_insts =  succ       -> insts ();
	  Inst           *target     =  succ_insts -> front ();

	  /* Don't take account of self loop: */
	  if (last_inst -> m_addr != target -> m_addr)
	    {
	      if (!last_inst -> m_branch)
		{
		  tr = fall_through (doc, last_inst, target, in_slice);
		  tmplt -> InsertEndChild (tr);
		  continue;
		}

	      if (target -> m_addr == last_inst -> m_addr +4)
		taken = false;
	      
	      tr = fall_into (doc, last_inst, target, in_slice, taken);
	      tmplt -> InsertEndChild (tr);
	    }
	}
    }
  
  nta -> InsertFirstChild (tmplt);
  nta -> InsertFirstChild (nta_decl);
  doc -> SaveFile (C(fn));
}

/* PRIVATE: */

ListDigraph::Node
CFG::findByLabel (string label)
{
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*m_bbs)[n];
      if (bb -> m_label == label)
	return n;
    }

  return INVALID;
}

void
CFG::findSuccs (vector<BB *> &bbs)
{
  ListDigraph::NodeIt m (*m_graph);
  for (; m != INVALID; ++m)
    {
      (*m_preds)[m] = new vector<BB *> ();
      (*m_succs)[m] = new vector<BB *> ();
    }
  
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    {
      BB   *bb   = (*m_bbs)[n];
      Inst *inst = bb -> m_insts -> back ();
      
      // TODO: hacky ; to clean
      size_t bclr_pos = inst -> m_disass.find ("bclr- ");
      if (bclr_pos != string::npos)
	continue;
      
      if (inst -> m_branch)
	{
	  ListDigraph::Node m = (*m_nodes)[inst -> m_target];
	  BB *cc = (*m_bbs)[m];
	  (*m_preds)[m] -> push_back (bb);
	  (*m_succs)[n] -> push_back (cc);

	  if (!inst -> m_uncond
	  &&   inst -> m_next != NULL)
	    {
	      ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
	      BB *cc = (*m_bbs)[m];
	      (*m_preds)[m] -> push_back (bb);
	      (*m_succs)[n] -> push_back (cc);
	    }
	}
      else
	{
	  if (inst -> m_next != NULL)
	    {
	      ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
	      BB *cc = (*m_bbs)[m];
	      (*m_preds)[m] -> push_back (bb);
	      (*m_succs)[n] -> push_back (cc);
	    }
	}
    }
}

void
CFG::deadcode_patch ()
{
  DFS *dfs = new DFS (this);
  vector<ListDigraph::Node> *to_erase = new vector<ListDigraph::Node> ();  
  ListDigraph::NodeIt n (*m_graph);
  for (; n != INVALID; ++n)
    if ((*dfs -> m_order)[n] == -1)
      to_erase -> push_back (n);

  {
  vector<ListDigraph::Node>::iterator n_it = to_erase -> begin ();
  for (; n_it != to_erase -> end (); ++n_it)
    {
      ListDigraph::Node n = *n_it;      
      ListDigraph::NodeIt m (*m_graph);
      for (; m != INVALID; ++m)
	{
	  BB *bb = (*m_bbs)[n];
	  vector<BB *> *preds = (*m_preds)[m];

	  vector<BB *>::iterator pos = find (preds -> begin (), preds -> end (), bb);
	  if (pos != preds -> end())
	    preds -> erase (pos);
	}
    }
  }
  {
  vector<ListDigraph::Node>::iterator n_it = to_erase -> begin ();
  for (; n_it != to_erase -> end (); ++n_it)
    {
      ListDigraph::Node n = *n_it;      
      ListDigraph::NodeIt m (*m_graph);
      for (; m != INVALID; ++m)
	{
	  BB *bb = (*m_bbs)[n];
	  vector<BB *> *succs = (*m_succs)[m];

	  vector<BB *>::iterator pos = find (succs -> begin (), succs -> end (), bb);
	  if (pos != succs -> end())
	    succs -> erase (pos);
	}
    }
  }
  
  vector<ListDigraph::Node>::iterator n_it = to_erase -> begin ();
  for (; n_it != to_erase -> end (); ++n_it)
    {
      ListDigraph::Node n = *n_it;      
      m_graph -> erase (n);
    }

  /////

  int i = 0;
  vector<Inst *> *sorted_insts = insts ();
  sort (sorted_insts -> begin (), sorted_insts -> end (), Inst::byAddr);
  vector<Inst *>::iterator inst_it = sorted_insts -> begin ();
  for (; inst_it != sorted_insts -> end (); ++inst_it, ++i)
    (*inst_it) -> m_num = i;
}

struct state {
  ListDigraph::Arc          arc;
  vector<ListDigraph::Node> stack;
};

void
CFG::blr_patch ()
{
  struct state state;
  ListDigraph::Arc first_arc;
  vector<ListDigraph::Node> stack_s;
  vector<struct state> w;
  vector<struct state> reached;
  
  m_graph -> firstOut (first_arc, m_entry);
  
  BB *bb = (*m_bbs)[m_entry];
  Inst *last_inst = bb -> m_insts -> back ();
  ListDigraph::Node ret = (*m_nodes)[last_inst -> m_addr +4];
  ListDigraph::Node trg = m_graph -> target (first_arc);
  stack_s.push_back (ret);

  state.arc = first_arc;
  state.stack = stack_s;
  w.push_back (state);
  
  while (!w.empty ())
    {
      vector<ListDigraph::Node> stack_t;
      
      // get next state and set as reached:
      struct state state = w.back (); w.pop_back ();
      reached.push_back (state);
      
      ListDigraph::Node src = m_graph -> source (state.arc);
      ListDigraph::Node trg = m_graph -> target (state.arc);
      BB *trg_bb = (*m_bbs)[trg];
      Inst *last_inst = trg_bb -> m_insts -> back ();

      // copy stack from state:
      vector<ListDigraph::Node>::iterator ret_it = state.stack.begin ();
      for (; ret_it != state.stack.end (); ++ret_it)
	{
	  ListDigraph::Node ret = *ret_it;
	  stack_t.push_back (ret);
	}

      // trg does a return:
      size_t bclr_pos = last_inst -> m_disass.find ("bclr- ");
      if (bclr_pos != string::npos)
	{
	  if (stack_t.empty ())
	    {
	      cerr << "SHOULD NOT BE EMPTY" << endl;
	      exit (1);
	    }
	  
	  ListDigraph::Node ret = stack_t.back ();
	  stack_t.pop_back ();
	  
	  BB *ret_bb = (*m_bbs)[ret];
	  Inst *ret_inst = ret_bb -> m_insts -> front ();
	  
	  bool exists_arc = false;
	  ListDigraph::OutArcIt arc (*m_graph, trg);
	  for (; arc != INVALID; ++arc)
	    if (m_graph -> target (arc) == ret)
	      exists_arc = true;
	  
	  if (!exists_arc)
	    {
	      addEdge (*trg_bb, *ret_bb);
	      (*m_preds)[ret] -> push_back (trg_bb);
	      (*m_succs)[trg] -> push_back (ret_bb);
	    }
	  
	  ListDigraph::Arc a;
	  m_graph -> firstOut (a, trg);
	  while (a != INVALID)
	    {
	      if (m_graph -> target (a) == ret)
		break;
	      
	      m_graph -> nextOut (a);
	    }
	  
	  state.arc = a;
	  state.stack = stack_t;
	  w.push_back (state);
	  
	  if (!last_inst -> m_uncond)
	    {
	      ListDigraph::Node target = (*m_nodes)[last_inst -> m_target];
	      BB *target_bb = (*m_bbs)[target];
	      bool exists_arc = false;
	      ListDigraph::OutArcIt arc (*m_graph, trg);
	      for (; arc != INVALID; ++arc)
		if (m_graph -> target (arc) == target)
		  exists_arc = true;
	      
	      if (!exists_arc)
		{
		  addEdge (*trg_bb, *target_bb);
		  (*m_preds)[target] -> push_back (trg_bb);
		  (*m_succs)[trg] -> push_back (target_bb);
		}

	      ListDigraph::Arc a;
	      m_graph -> firstOut (a, trg);
	      while (a != INVALID)
		{
		  if (m_graph -> target (a) == target)
		    break;
		  
		  m_graph -> nextOut (a);
		}
	  
	      state.arc = a;
	      state.stack = stack_t;
	      state.stack.push_back (ret);
	      w.push_back (state);
	    }
	  
	  // TODO: hacky ; to clean
	  //last_inst -> m_disass = "blr";
	  last_inst -> m_refs   &= ~0xa0; // unsets pc and lr.
	  last_inst -> m_defs   &= ~0xa0; // id.
	  last_inst -> m_link   = false;
	  last_inst -> m_uncond = true;
	  last_inst -> m_target = ret_inst -> m_addr;
	}
      else
	{
	  // trg does a call:
	  size_t bl_pos = last_inst -> m_disass.find ("bl ");
	  if (bl_pos != string::npos)
	    {
	      ListDigraph::Node ret = (*m_nodes)[last_inst -> m_addr +4];
	      stack_t.push_back (ret);
	    } 

	  // add states too w if not yet reached:
	  ListDigraph::Arc arc;
	  m_graph -> firstOut (arc, trg);
	  state.stack = stack_t;
	  while (arc != INVALID)
	    {
	      ListDigraph::Node u = m_graph -> target (arc);
	      state.arc = arc;
	      
	      bool found = false;
	      vector<struct state>::iterator reach_it = reached.begin ();
	      for (; reach_it != reached.end (); ++reach_it)
		{
		  struct state reach = *reach_it;	      
		  if (state.arc == reach.arc)
		    {
		      if (state.stack.size () == reach.stack.size ())
			{
			  bool equals = true;
			  vector<ListDigraph::Node>::iterator state_ret_it = state.stack.begin ();
			  vector<ListDigraph::Node>::iterator reach_ret_it = reach.stack.begin ();
			  for (; state_ret_it != state.stack.end (); ++state_ret_it, ++reach_ret_it)
			    {
			      ListDigraph::Node state_ret = *state_ret_it;
			      ListDigraph::Node reach_ret = *reach_ret_it;
			      if (state_ret != reach_ret)
				{
				  equals = false;
				  break;
				}
			    }
			  
			  if (equals)
			    {
			      found = true;
			      break;
			    }
			}
		    }
		}
	  
	      if (!found)
		w.push_back (state);
	      
	      m_graph -> nextOut (arc);
	    }
	}
    }
}
