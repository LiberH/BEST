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
  u32 entry_addr, exit_addr;
  
  CFG          *cfg = new CFG ();
  vector<BB *> *bbs = BB::FromFile (f, &entry_addr, &exit_addr);
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
	      Agnode_t *nxt = agnode (agraph, C(inst -> m_next -> m_name), TRUE);
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

      /*
      vector<Inst *> *insts = cfg -> insts ();
      sort (insts -> begin (), insts -> end (), Inst::byAddr);
      vector<Inst *>::iterator inst_it = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  cout << hex << inst -> m_addr << ": " << inst -> m_disass << endl;
	  if      (!inst -> m_branch) cout << " next   : " << inst -> m_next -> m_addr << endl;
	  else if ( inst -> m_uncond) cout << " target : " << inst -> m_target << endl;
	  else                      { cout << " next   : " << inst -> m_next -> m_addr << endl;
                                      cout << " target : " << inst -> m_target << endl; }
	  cout << endl;
	}
      */
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
  ostringstream oss;
  
  oss.str (""); oss << "id" << hex << src_inst -> m_addr; src = oss.str ();
  oss.str (""); oss << "id" << hex << trg_inst -> m_addr; trg = oss.str ();
  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t tr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
  
  if (in_slice)
  { oss.str (""); oss << "execute_"        << hex << src_inst -> m_addr << "()"; upd = oss.str (); }
    oss.str (""); oss << "IMU_IsAccessed(" << dec << src_inst -> m_num << ")";   grd = oss.str ();
  
  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL              , tr_src_attrs , NULL);
  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL              , tr_trg_attrs , NULL);
  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd)            , tr_grd_attrs , NULL);
  XMLElement *tr_syn = newElementWrapper (doc, "label"  , "IMU_doneAccess?" , tr_syn_attrs , NULL);
  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd)            , tr_upd_attrs , NULL);
  
  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_syn, tr_upd, NULL};
  tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
  
  return tr;
}

XMLElement *
CFG::pre_jump (XMLDocument *doc, Inst *src_inst, string trg_id, bool taken)
{
  XMLElement *tr;
  string src, trg, grd, upd;
  ostringstream oss;
  
  oss.str (""); oss << "id" << hex << src_inst -> m_addr; src = oss.str ();
  oss.str (""); oss << "id" << trg_id;                    trg = oss.str ();
  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
  
  grd = (taken ? "" : "!");
  upd = "_taken = ";
  upd += (taken ? "true" : "false");
  switch (src_inst -> m_test)
    {
    case  0: grd += "true"; break;
      
    case  1: grd += "lt()"; break;
    case  2: grd += "gt()"; break;
    case  3: grd += "eq()"; break;
    case  4: grd += "so()"; break;
      
    case  5: grd += "ge()"; break;
    case  6: grd += "le()"; break;
    case  7: grd += "ne()"; break;
      
    case  9: grd +=  "z()"; break;
    case 10: grd += "nz()"; break;
      
    default: grd += "####"; break;
    }
  
  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL   , tr_src_attrs , NULL);
  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL   , tr_trg_attrs , NULL);
  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd) , tr_grd_attrs , NULL);
  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd) , tr_upd_attrs , NULL);
  
  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_upd, NULL};
  tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
  
  return tr;
}

XMLElement *
CFG::jump (XMLDocument *doc, string src_id, Inst *src_inst, Inst *trg_inst, bool in_slice)
{
  XMLElement *tr;
  string src, trg, grd, upd;
  ostringstream oss;
  
  oss.str (""); oss << "id" << src_id;                    src = oss.str ();
  oss.str (""); oss << "id" << hex << trg_inst -> m_addr; trg = oss.str ();
  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t tr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
  
  oss.str (""); oss << "IMU_IsAccessed(" << dec << src_inst -> m_num << ")"; grd = oss.str ();
  if (in_slice)
    { oss.str (""); oss << "execute_" << hex << src_inst -> m_addr << "()"; upd = oss.str (); }
  
  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL              , tr_src_attrs , NULL);
  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL              , tr_trg_attrs , NULL);
  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd)            , tr_grd_attrs , NULL);
  XMLElement *tr_syn = newElementWrapper (doc, "label"  , "IMU_doneAccess?" , tr_syn_attrs , NULL);
  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd)            , tr_upd_attrs , NULL);
  
  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_syn, tr_upd, NULL};
  tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
  
  return tr;
}

static
string reg_names[62] = {
  "cr"      , "ctr"   , "l1csr0" , "l1csr1" , "l1finv1" , "lr"   , "msr"  , "pc"   ,
  "serial0" , "srr0"  , "srr1"   , "xer"    , "hit"     , "miss" , "fpr0" , "fpr1" ,
  "fpr2"    , "fpr3"  , "fpr4"   , "fpr5"   , "fpr6"    , "fpr7" , "fpr8" , "fpr9" ,
  "fpr10"   , "fpr11" , "fpr12"  , "fpr13"  , "fpr14"  , "fpr15" ,
  
  "r0"  , "r1"  , "r2"  , "r3"  , "r4"  , "r5"  , "r6"  , "r7"  ,
  "r8"  , "r9"  , "r10" , "r11" , "r12" , "r13" , "r14" , "r15" ,
  "r16" , "r17" , "r18" , "r19" , "r20" , "r21" , "r22" , "r23" ,
  "r24" , "r25" , "r26" , "r27" , "r28" , "r29" , "r30" , "r31" };

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

  // <declaration>
  XMLElement *nta_decl = nta -> FirstChildElement ("declaration");
  const char *nta_decl_txt = nta_decl -> GetText ();

  oss.str ("");
  vector<Inst *> *insts = cfg -> insts ();
  u64 refs = 0;
  vector<Inst *>::iterator inst_it = slice -> begin ();  
  for (; inst_it != slice -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
	refs |= inst -> m_refs;
    }

  oss << "int";
  bitset<64> bs (refs | 0b100000000011);
  for (int b = 0; b < 62; ++b)
    if (b != 7 && bs[b])
      oss << " " << reg_names[b] << ",";
  oss.seekp (-1, oss.cur);
  oss << ";" << endl;
  oss << endl;

  int n_insts = insts -> size ();
  oss << "const int _REGS_MAX = 8;" << endl;
  oss << "const int _INST_MAX = " << n_insts +32 << ";" << endl;
  oss << "typedef struct {"               << endl;
  oss << "  int  addr;"                  << endl;
  oss << "  int  cycles;"                << endl;
  oss << "  bool do_branch;"             << endl;
  oss << "  int  target;"                << endl;
  oss << "  bool do_memory;"             << endl;
  oss << "  int  read_regs[_REGS_MAX];"  << endl;
  oss << "  int  write_regs[_REGS_MAX];" << endl;
  oss << "} _Inst_t;"                    << endl;
  oss << endl;
  oss << "const _Inst_t _INSTS[_INST_MAX] = {";

  int last_addr = -1;
  vector<BB *> *bbs = cfg -> bbs ();
  sort (bbs -> begin (), bbs -> end (), BB::byAddr);
  vector<BB *>::iterator bb_it = bbs -> begin ();  
  for (; bb_it != bbs -> end (); ++bb_it)
    //ListDigraph::NodeIt o (*cfg -> m_graph);
    //for (; o != INVALID; ++o)
    {
      BB *bb = *bb_it;

      oss << endl;
      oss << endl << "  /* " << bb -> m_label << " */";

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
	  
	  oss << endl;
	  oss << "  /*  " << disass << spaces << " - " << dec << inst -> m_num << " */ { ";
	  oss << dec << inst -> m_addr                    << ", ";
	  oss << 1                                        << ", "; // latency;
	  oss << (inst -> m_branch ? "true, " : "false,") << " ";
	  if (inst -> m_branch)
	    {
	      o.str ("");
	      o << dec << target_num;
	      spaces = string (9 - o.str ().length (), ' ');
	      oss << spaces << dec << target_num;
	    }
	  else oss << "_INST_MAX";
	  oss << ", ";
	  oss << (inst -> m_memory ? "true, " : "false,") << " "; // does_mem_access
	  oss << "{ ";

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
		oss << (first ? "" : ", ") << spaces << byte;
		first = false;
	      }
	    while (i);
	  }
	  
	  oss << " }, { ";

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
		oss << (first ? "" : ", ") << spaces << byte;
		first = false;
	      }
	    while (i);
	  }
	  
	  oss << " } },";
	}
    }

  oss << endl;
  oss << endl << "  /* Nops */";
  for (int i = 0; i < 32; ++i)
    {
      oss << endl << "  /*  xxxx: ---                      - " << dec << n_insts +i << " */";
      oss << " { " << dec << (last_addr + (i +1)*4) << ", 1, false, _INST_MAX, false,";
      oss << " {   0,   0,   0,   0,   0,   0,   0,   0 },";
      oss << " {   0,   0,   0,   0,   0,   0,   0,   0 } },";
    }
  
  oss.seekp (-1, oss.cur);
  oss << endl << "};" << endl;
  oss << endl;
  oss << nta_decl_txt << endl;

  inst_it = slice -> begin ();  
  for (; inst_it != slice -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      oss << "void execute_" << hex << inst -> m_addr << "() {"
	  << " " << inst -> m_function << "; "
	  << "}" << endl;
    }
  
/*
// TODO: generate from tool (currently hand written)
void execute_3004() { li(r10, 3);        }
void execute_300c() { li(r7, 4);         } // TODO: modified for simulation
					   // ease; set back to "28" for real
					   // testing
void execute_3010() { mtctr(r7);         }
void execute_3014() { cmpi(cr, r3, 1);   }
void execute_3018() { bgt(12340);        }
void execute_3024() { addi(r10, r10, 1); }
void execute_302c() { bdz(12352);        }
void execute_3034() { cmpw(cr, r3, r10); }
void execute_3038() { bge(12320);        }
void execute_3058() { li(r3, 30);        }
*/

  /*
  oss << "/ * Functions: * /" << endl;
  oss << endl;
  oss << "int dumb;" << endl;
  inst_it = slice -> begin ();
  for (; inst_it != slice -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;
      if (inst -> m_branch) continue;
      
      oss << "void execute_" << hex << inst -> m_addr << "() {" << endl;
      oss << "  // TODO" << endl;
      oss << "  dumb = 0;" << endl;
      oss << "}" << endl;
      oss << endl;
    }
  oss.seekp (-1, oss.cur);
  */
  
  nta_decl -> SetText (C(oss.str ()));
  // </declaration>
  
  //////

  // <tempalte>
  XMLElement *tmplt = nta -> FirstChildElement ("template");
  //XMLElement *tmplt = doc -> NewElement ("template");
  //XMLElement *tmplt_name = doc -> NewElement ("name");  
  //tmplt_name -> SetText ("Binary");
  //tmplt -> InsertEndChild (tmplt_name);
  
  //XMLElement *tmplt_decl = doc -> NewElement ("declaration");
  //tmplt -> InsertEndChild (tmplt_decl);

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

  /*
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB   *bb       = (*cfg -> m_bbs)[n];
      Inst *bb_entry = bb -> m_insts -> front ();
      Inst *bb_exit  = bb -> m_insts -> back ();

      vector<Inst *>          *insts   = bb    -> m_insts;
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

	  if (inst == bb_exit
           && inst -> m_branch)
	    {
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_";
	      attr_t attrs[] = {{"id"    , C(oss.str ()) },
				{"x"     , C(S(inst -> m_x))},
				{"y"     , C(S(inst -> m_y))},
				{"color" , "#ffc0cb"     },
				{NULL    , NULL          }};
	      XMLElement *loc_comm = newElementWrapper (doc, "committed", NULL, NULL, NULL);
	      XMLElement *loc_childs[] = {(loc_name ? loc_name : loc_comm),
					  (loc_name ? loc_comm : NULL    ), NULL};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, loc_childs);
	      tmplt -> InsertEndChild (loc);
	      loc_name = NULL;

	      /////
	      
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "true";
	      attr_t true_attrs[] = {{"id" , C(oss.str ()) },
				     {"x"  , C(S(inst -> m_x))},
				     {"y"  , C(S(inst -> m_y))},
				     {NULL , NULL          }};
	      XMLElement *true_loc_childs[] = {loc_name, NULL};
	      XMLElement *true_loc = newElementWrapper (doc, "location", NULL, true_attrs, true_loc_childs);
	      tmplt -> InsertEndChild (true_loc);
	      
	      if (inst -> m_test)
		{ 
		  oss.str (""); oss << "id" << hex << inst -> m_addr << "false";
		  attr_t false_attrs[] = {{"id" , C(oss.str ()) },
					  {"x"  , C(S(inst -> m_x))},
					  {"y"  , C(S(inst -> m_y))},
					  {NULL , NULL          }};
		  XMLElement *false_loc_childs[] = {loc_name, NULL};
		  XMLElement *false_loc = newElementWrapper (doc, "location", NULL, false_attrs, false_loc_childs);
		  tmplt -> InsertEndChild (false_loc);
		}
	      
	      continue;
	    }

	  oss.str (""); oss << "id" << hex << inst -> m_addr;
	  attr_t attrs[] = {{"id" , C(oss.str ()) },
			    {"x"  , C(S(inst -> m_x))},
			    {"y"  , C(S(inst -> m_y))},
			    {NULL , NULL          }};
	  XMLElement *loc_childs[] = {loc_name, NULL};
	  XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, loc_childs);
	  tmplt -> InsertEndChild (loc);
	}
    }
  */

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

	  if (!inst -> m_branch)
	    {
	      oss.str (""); oss << "id" << hex << inst -> m_addr;
	      attr_t attrs[] = {{"id" , C(oss.str ())    },
				{"x"  , C(S(inst -> m_x))},
				{"y"  , C(S(inst -> m_y))},
				{NULL , NULL             }};
	      XMLElement *loc_childs[] = {loc_name, NULL};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, loc_childs);
	      tmplt -> InsertEndChild (loc);
	    }
	  else if (inst -> m_uncond)
	    {
	      {
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_comm";
	      attr_t attrs[] = {{"id" , C(oss.str ())    },
				{"x"  , C(S(inst -> m_x))},
				{"y"  , C(S(inst -> m_y))},
				{NULL , NULL             }};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, NULL);
	      tmplt -> InsertEndChild (loc);
	      }
	      /////
	      {
	      oss.str (""); oss << "id" << hex << inst -> m_addr;
	      attr_t attrs[] = {{"id"    , C(oss.str ()) },
				{"x"     , C(S(inst -> m_x))},
				{"y"     , C(S(inst -> m_y))},
				{"color" , "#ffc0cb"     },
				{NULL    , NULL          }};
	      XMLElement *loc_comm = newElementWrapper (doc, "committed", NULL, NULL, NULL);
	      XMLElement *loc_childs[] = {(loc_name ? loc_name : loc_comm),
					  (loc_name ? loc_comm : NULL    ), NULL};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, loc_childs);
	      tmplt -> InsertEndChild (loc);
	      }
	    }
	  else
	    {
	      {
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_true";
	      attr_t attrs[] = {{"id" , C(oss.str ())    },
				{"x"  , C(S(inst -> m_x))},
				{"y"  , C(S(inst -> m_y))},
				{NULL , NULL             }};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, NULL);
	      tmplt -> InsertEndChild (loc);
	      }
	      /////
	      {
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_false";
	      attr_t attrs[] = {{"id" , C(oss.str ())    },
				{"x"  , C(S(inst -> m_x))},
				{"y"  , C(S(inst -> m_y))},
				{NULL , NULL             }};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, NULL);
	      tmplt -> InsertEndChild (loc);
	      }
	      /////
	      {
	      oss.str (""); oss << "id" << hex << inst -> m_addr;
	      attr_t attrs[] = {{"id"    , C(oss.str ()) },
				{"x"     , C(S(inst -> m_x))},
				{"y"     , C(S(inst -> m_y))},
				{"color" , "#ffc0cb"     },
				{NULL    , NULL          }};
	      XMLElement *loc_comm = newElementWrapper (doc, "committed", NULL, NULL, NULL);
	      XMLElement *loc_childs[] = {(loc_name ? loc_name : loc_comm),
					  (loc_name ? loc_comm : NULL    ), NULL};
	      XMLElement *loc = newElementWrapper (doc, "location", NULL, attrs, loc_childs);
	      tmplt -> InsertEndChild (loc);
	      }
	    }
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
  
  oss.str (""); oss << "InCU.PC = " << dec << entry -> m_num << ",\n_clock = 0";
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

  oss.str (""); oss << "id" << hex << exit -> m_addr << "_comm";
  attr_t ltr_src_attrs[] = {{"ref"  , C(oss.str ())    } , {NULL , NULL}};
  attr_t ltr_trg_attrs[] = {{"ref"  , "idexit"         } , {NULL , NULL}};
  attr_t ltr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
  attr_t ltr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
  attr_t ltr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
  
  oss.str (""); oss << "IMU_IsAccessed(" << dec << exit -> m_num << ")";
  XMLElement *ltr_src = newElementWrapper (doc, "source" , NULL                    , ltr_src_attrs , NULL);
  XMLElement *ltr_trg = newElementWrapper (doc, "target" , NULL                    , ltr_trg_attrs , NULL);
  XMLElement *ltr_grd = newElementWrapper (doc, "label"  , C(oss.str ())           , ltr_grd_attrs , NULL);
  XMLElement *ltr_syn = newElementWrapper (doc, "label"  , "IMU_doneAccess?"       , ltr_syn_attrs , NULL);
  XMLElement *ltr_upd = newElementWrapper (doc, "label"  , "_mustTerminate = true" , ltr_upd_attrs , NULL);

  XMLElement *ltr_childs[] = {ltr_src, ltr_trg, ltr_grd, ltr_syn, ltr_upd, NULL};
  XMLElement *ltr = newElementWrapper (doc, "transition", NULL, NULL, ltr_childs);
  tmplt -> InsertEndChild (ltr);

  ////////////////////////
  // Other transitions: //
  ////////////////////////

  {
  vector<Inst *> *insts = cfg -> insts ();
  sort (insts -> begin (), insts -> end (), Inst::byAddr);
  vector<Inst *>::iterator inst_it = insts -> begin ();
  for (; inst_it != insts -> end (); ++inst_it)
    {
      Inst *inst = *inst_it;      
      string src, trg, grd, upd;

      if (!inst -> m_branch)
	{
	  bool in_slice;
	  XMLElement *tr;
	  
	  in_slice = false;
	  vector<Inst *>::iterator slice_it = find (slice -> begin (), slice -> end (), inst);
	  if (slice_it != slice -> end ())
	      in_slice = true;
	  
	  tr = fall_through (doc, inst, inst -> m_next, in_slice);
	  tmplt -> InsertEndChild (tr);
	}
      else if (inst -> m_uncond)
	{
	  if (inst -> m_target == inst -> m_addr)
	    {
	      XMLElement *tr;
	      ostringstream oss;
	      
	      oss << hex << inst -> m_addr << "_comm";
	      tr = pre_jump (doc, inst, oss.str (), true);
	      tmplt -> InsertEndChild (tr);
	      
	      continue;
	    }
	  
	  Inst *target = NULL;
	  vector<Inst *> *targets = cfg -> insts ();
	  vector<Inst *>::iterator target_it = targets -> begin ();
	  for (; target_it != targets -> end (); ++target_it)
	    {
	      target = *target_it;
	      if (target -> m_addr == inst -> m_target)
		break;
	    }

	  bool in_slice;
	  XMLElement *tr;
	  ostringstream oss;

	  oss << hex << inst -> m_addr << "_comm";
	  tr = pre_jump (doc, inst, oss.str (), true);
	  tmplt -> InsertEndChild (tr);

	  in_slice = false;
	  vector<Inst *>::iterator slice_it = find (slice -> begin (), slice -> end (), inst);
	  if (slice_it != slice -> end ())
	      in_slice = true;
	  
	  tr = jump (doc, oss.str (), inst, target, in_slice);
	  tmplt -> InsertEndChild (tr);
	}
      else
	{	  
	  bool in_slice;
	  XMLElement *tr;
	  ostringstream oss;

	  oss << hex << inst -> m_addr << "_false";
	  tr = pre_jump (doc, inst, oss.str (), false);
	  tmplt -> InsertEndChild (tr);

	  in_slice = false;
	  vector<Inst *>::iterator slice_it = find (slice -> begin (), slice -> end (), inst);
	  if (slice_it != slice -> end ())
	      in_slice = true;
	  
	  tr = jump (doc, oss.str (), inst, inst -> m_next, in_slice);
	  tmplt -> InsertEndChild (tr);

	  /////

	  Inst *target = NULL;
	  vector<Inst *> *targets = cfg -> insts ();
	  vector<Inst *>::iterator target_it = targets -> begin ();
	  for (; target_it != targets -> end (); ++target_it)
	    {
	      target = *target_it;
	      if (target -> m_addr == inst -> m_target)
		break;
	    }

	  oss.str ("");
	  oss << hex << inst -> m_addr << "_true";
	  tr = pre_jump (doc, inst, oss.str (), true);
	  tmplt -> InsertEndChild (tr);

	  in_slice = false;
	  slice_it = find (slice -> begin (), slice -> end (), inst);
	  if (slice_it != slice -> end ())
	      in_slice = true;
	  
	  tr = jump (doc, oss.str (), inst, target, in_slice);
	  tmplt -> InsertEndChild (tr);
	}
    }
  }
  
  /*
  ListDigraph::NodeIt m (*cfg -> m_graph);
  for (; m != INVALID; ++m)
    {
      BB   *bb       = (*cfg -> m_bbs)[m];
      Inst *bb_entry = bb -> m_insts -> front ();
      Inst *bb_exit  = bb -> m_insts -> back  ();
      Inst *bb_prev  = NULL;

      // Intra-BB transistions:
      
      vector<Inst *>          *insts   = bb    -> m_insts;
      vector<Inst *>::iterator inst_it = insts -> begin ();  
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;

	  // Special case: one-instruction BB.
	  if (inst == bb_entry
	   && inst == bb_exit
	   && inst -> m_branch)
	    {
	      string src, trg, grd, upd;
		  		  
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_"; src = oss.str ();
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "true"; trg = oss.str ();
	      attr_t tr_src_attrs[] = {{"ref"  , C(src)       } , {NULL , NULL}};
	      attr_t tr_trg_attrs[] = {{"ref"  , C(trg)       } , {NULL , NULL}};
	      attr_t tr_grd_attrs[] = {{"kind" , "guard"      } , {NULL , NULL}};  
	      attr_t tr_upd_attrs[] = {{"kind" , "assignment" } , {NULL , NULL}};

	      bool taken = true;
	      if ((inst -> m_test)
	       &&  inst -> m_target == inst -> m_addr +4)
		taken = false;
	      
	      grd = (taken ? "" : "!");
	      upd = "_taken = ";
	      upd += (taken ? "true" : "false");
	      switch (inst -> m_test)
		{
		case  0: grd += "true"; break;
		  
		case  1: grd += "lt()"; break;
		case  2: grd += "gt()"; break;
		case  3: grd += "eq()"; break;
		case  4: grd += "so()"; break;
		  
		case  5: grd += "ge()"; break;
		case  6: grd += "le()"; break;
		case  7: grd += "ne()"; break;
		  
		case  9: grd +=  "z()"; break;
		case 10: grd += "nz()"; break;
		  
		default: grd += "####"; break;
		}
	      
	      XMLElement *tr_src = newElementWrapper (doc, "source" , NULL   , tr_src_attrs , NULL);
	      XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL   , tr_trg_attrs , NULL);
	      XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd) , tr_grd_attrs , NULL);
	      XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd) , tr_upd_attrs , NULL);
	      
	      XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_upd, NULL};
	      XMLElement *tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
	      tmplt -> InsertEndChild (tr);
	      
	      bb_prev = inst;
	      continue;
	    }
	  
	  // Special case: instruction is BB entry point.
	  if (inst == bb_entry)
	    {
	      bb_prev = inst;
	      continue;
	    }
	  
	  // Special case: instruction is BB exit point.
	  if (inst == bb_exit)
	    {
	      string src, trg, grd, upd;
	      
	      /////
	      
	      oss.str (""); oss << "id" << hex << bb_prev -> m_addr;        src = oss.str ();
	      oss.str (""); oss << "id" << hex << inst    -> m_addr << "_"; trg = oss.str ();
	      attr_t tr0_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
	      attr_t tr0_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
	      attr_t tr0_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
	      attr_t tr0_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
	      attr_t tr0_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
	      
	      oss.str (""); oss << "IMU_IsAccessed(" << dec << bb_prev -> m_num << ")"; grd = oss.str ();
	      if (find (slice -> begin (), slice -> end (), bb_prev ) != slice -> end ())
		{ oss.str (""); oss << "execute_" << hex << bb_prev -> m_addr << "()"; upd = oss.str (); }
	      
	      XMLElement *tr0_src = newElementWrapper (doc, "source" , NULL              , tr0_src_attrs , NULL);
	      XMLElement *tr0_trg = newElementWrapper (doc, "target" , NULL              , tr0_trg_attrs , NULL);
	      XMLElement *tr0_grd = newElementWrapper (doc, "label"  , C(grd)            , tr0_grd_attrs , NULL);
	      XMLElement *tr0_syn = newElementWrapper (doc, "label"  , "IMU_doneAccess?" , tr0_syn_attrs , NULL);
	      XMLElement *tr0_upd = newElementWrapper (doc, "label"  , C(upd)            , tr0_upd_attrs , NULL);
	      
	      XMLElement *tr0_childs[] = {tr0_src, tr0_trg, tr0_grd, tr0_syn, tr0_upd, NULL};
	      XMLElement *tr0 = newElementWrapper (doc, "transition", NULL, NULL, tr0_childs);
	      tmplt -> InsertEndChild (tr0);
	      
	      /////
		  
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_";    src = oss.str ();
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "true"; trg = oss.str ();
	      attr_t tr1_src_attrs[] = {{"ref"  , C(src)       } , {NULL , NULL}};
	      attr_t tr1_trg_attrs[] = {{"ref"  , C(trg)       } , {NULL , NULL}};
	      attr_t tr1_grd_attrs[] = {{"kind" , "guard"      } , {NULL , NULL}};  
	      attr_t tr1_upd_attrs[] = {{"kind" , "assignment" } , {NULL , NULL}};

	      grd = ""; upd = "";
	      //if (inst -> m_branch)
		{
		  grd = "";
		  upd = "_taken = true";
		  switch (inst -> m_test)
		    {
		    case  0: grd += "true"; break;
		      
		    case  1: grd += "lt()"; break;
		    case  2: grd += "gt()"; break;
		    case  3: grd += "eq()"; break;
		    case  4: grd += "so()"; break;

		    case  5: grd += "ge()"; break;
		    case  6: grd += "le()"; break;
		    case  7: grd += "ne()"; break;

		    case  9: grd +=  "z()"; break;
		    case 10: grd += "nz()"; break;
		      
		    default: grd += "####"; break;
		    }
		}

	      XMLElement *tr1_src = newElementWrapper (doc, "source" , NULL   , tr1_src_attrs , NULL);
	      XMLElement *tr1_trg = newElementWrapper (doc, "target" , NULL   , tr1_trg_attrs , NULL);
	      XMLElement *tr1_grd = newElementWrapper (doc, "label"  , C(grd) , tr1_grd_attrs , NULL);
	      XMLElement *tr1_upd = newElementWrapper (doc, "label"  , C(upd) , tr1_upd_attrs , NULL);
	      
	      XMLElement *tr1_childs[] = {tr1_src, tr1_trg, tr1_grd, tr1_upd, NULL};
	      XMLElement *tr1 = newElementWrapper (doc, "transition", NULL, NULL, tr1_childs);
	      tmplt -> InsertEndChild (tr1);
	      
	      /////
		  
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "_";     src = oss.str ();
	      oss.str (""); oss << "id" << hex << inst -> m_addr << "false"; trg = oss.str ();
	      attr_t tr2_src_attrs[] = {{"ref"  , C(src)       } , {NULL , NULL}};
	      attr_t tr2_trg_attrs[] = {{"ref"  , C(trg)       } , {NULL , NULL}};
	      attr_t tr2_grd_attrs[] = {{"kind" , "guard"      } , {NULL , NULL}};  
	      attr_t tr2_upd_attrs[] = {{"kind" , "assignment" } , {NULL , NULL}};

	      grd = ""; upd = "";
	      //if (inst -> m_branch)
		{
		  grd = "!";
		  upd = "_taken = false";
		  switch (inst -> m_test)
		    {
		    case  0: grd += "true"; break;
		      
		    case  1: grd += "lt()"; break;
		    case  2: grd += "gt()"; break;
		    case  3: grd += "eq()"; break;
		    case  4: grd += "so()"; break;

		    case  5: grd += "ge()"; break;
		    case  6: grd += "le()"; break;
		    case  7: grd += "ne()"; break;

		    case  9: grd +=  "z()"; break;
		    case 10: grd += "nz()"; break;
		      
		    default: grd += "####"; break;
		    }
		}

	      XMLElement *tr2_src = newElementWrapper (doc, "source" , NULL   , tr2_src_attrs , NULL);
	      XMLElement *tr2_trg = newElementWrapper (doc, "target" , NULL   , tr2_trg_attrs , NULL);
	      XMLElement *tr2_grd = newElementWrapper (doc, "label"  , C(grd) , tr2_grd_attrs , NULL);
	      XMLElement *tr2_upd = newElementWrapper (doc, "label"  , C(upd) , tr2_upd_attrs , NULL);
	      
	      XMLElement *tr2_childs[] = {tr2_src, tr2_trg, tr2_grd, tr2_upd, NULL};
	      XMLElement *tr2 = newElementWrapper (doc, "transition", NULL, NULL, tr2_childs);
	      tmplt -> InsertEndChild (tr2);
	      
	      bb_prev = inst;
	      continue;
	    }

	  // Standard case.
	  string src, trg, grd, upd;
		  
	  oss.str (""); oss << "id" << hex << bb_prev -> m_addr; src = oss.str ();
	  oss.str (""); oss << "id" << hex << inst    -> m_addr; trg = oss.str ();
	  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
	  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
	  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
	  attr_t tr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
	  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
	  
	  oss.str (""); oss << "IMU_IsAccessed(" << dec << bb_prev -> m_num << ")"; grd = oss.str ();
	  if (find (slice -> begin (), slice -> end (), bb_prev ) != slice -> end ())
	    { oss.str (""); oss << "execute_" << hex << bb_prev -> m_addr << "()"; upd = oss.str (); }
	  
	  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL              , tr_src_attrs , NULL);
	  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL              , tr_trg_attrs , NULL);
	  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd)            , tr_grd_attrs , NULL);
	  XMLElement *tr_syn = newElementWrapper (doc, "label"  , "IMU_doneAccess?" , tr_syn_attrs , NULL);
	  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd)            , tr_upd_attrs , NULL);
	  
	  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_syn, tr_upd, NULL};
	  XMLElement *tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
	  tmplt -> InsertEndChild (tr);	  
	  bb_prev = inst;
	}
      
      // Inter-BBs transistions:
      
      vector<BB *> *next_bbs = (*cfg -> m_succs)[m];
      vector<BB *>::iterator next_bb_it = next_bbs -> begin ();
      for (; next_bb_it != next_bbs -> end (); ++next_bb_it)
	{
	  BB             *next_bb    = *next_bb_it;
	  vector<Inst *> *next_insts =  next_bb -> m_insts;
	  Inst           *next_inst  =  next_insts -> front ();

	  // remove last self loop
	  if (bb_exit -> m_next == NULL)
	    continue;

	  string src, trg, grd, upd;

	  bool taken = true;
	  if (next_inst -> m_addr == bb_exit -> m_addr +4)
	    taken = false;
	    
	  oss.str (""); oss << "id" << hex << bb_exit   -> m_addr;
	  if (bb_exit -> m_branch)
	    oss << (bb_exit -> m_test ? (taken ? "true" : "false") : "true");
	  src = oss.str ();
	  oss.str (""); oss << "id" << hex << next_inst -> m_addr; trg = oss.str ();
	  if (next_inst  -> m_branch
	   && next_insts -> size () == 1)
	    trg += "_";
	  attr_t tr_src_attrs[] = {{"ref"  , C(src)           } , {NULL , NULL}};
	  attr_t tr_trg_attrs[] = {{"ref"  , C(trg)           } , {NULL , NULL}};
	  attr_t tr_grd_attrs[] = {{"kind" , "guard"          } , {NULL , NULL}};  
	  attr_t tr_syn_attrs[] = {{"kind" , "synchronisation"} , {NULL , NULL}};  
	  attr_t tr_upd_attrs[] = {{"kind" , "assignment"     } , {NULL , NULL}};
	  
	  oss.str (""); oss << "IMU_IsAccessed(" << dec << bb_prev -> m_num << ")"; grd = oss.str ();
	  if (find (slice -> begin (), slice -> end (), bb_prev ) != slice -> end ())
	    { oss.str (""); oss << "execute_" << hex << bb_prev -> m_addr << "()"; upd = oss.str (); }
	  
	  XMLElement *tr_src = newElementWrapper (doc, "source" , NULL              , tr_src_attrs , NULL);
	  XMLElement *tr_trg = newElementWrapper (doc, "target" , NULL              , tr_trg_attrs , NULL);
	  XMLElement *tr_grd = newElementWrapper (doc, "label"  , C(grd)            , tr_grd_attrs , NULL);
	  XMLElement *tr_syn = newElementWrapper (doc, "label"  , "IMU_doneAccess?" , tr_syn_attrs , NULL);
	  XMLElement *tr_upd = newElementWrapper (doc, "label"  , C(upd)            , tr_upd_attrs , NULL);
	  
	  XMLElement *tr_childs[] = {tr_src, tr_trg, tr_grd, tr_syn, tr_upd, NULL};
	  XMLElement *tr = newElementWrapper (doc, "transition", NULL, NULL, tr_childs);
	  tmplt -> InsertEndChild (tr);
	}
    }
  */
  nta -> InsertFirstChild (tmplt);
  nta -> InsertFirstChild (nta_decl);
  // </template>

  // TODO: move to ppc-no_binary.xml
  /*
  vector<string> queries;
  queries.push_back ("sup: wcet");
  queries.push_back ("sup: fetch_count");
  queries.push_back ("sup: stall_count");
  queries.push_back ("sup: cache_default");
  queries.push_back ("sup: memory_access");
  
  XMLElement *nta_queries = nta -> FirstChildElement ("queries");
  vector<string>::iterator query_it = queries.begin ();
  for (; query_it != queries.end (); ++query_it)
    {
      string query = *query_it;
      
      XMLElement *q_query   = doc -> NewElement ("query");
      XMLElement *q_formula = doc -> NewElement ("formula");
      
      q_formula   -> SetText (C(query));
      q_query     -> InsertEndChild (q_formula);
      nta_queries -> InsertEndChild (q_query);
    }
  
  nta -> InsertEndChild (nta_queries);
  */
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
	  
	  if (!inst -> m_uncond)
	    if (inst -> m_next)
	      {
		ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
		BB *cc = (*m_bbs)[m];
		(*m_preds)[m] -> push_back (bb);
		(*m_succs)[n] -> push_back (cc);
	      }
	}
      else
	if (inst -> m_next)
	  {
	    ListDigraph::Node m = (*m_nodes)[inst -> m_next -> m_addr];
	    BB *cc = (*m_bbs)[m];
	    (*m_preds)[m] -> push_back (bb);
	    (*m_succs)[n] -> push_back (cc);
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
}

struct state {
  ListDigraph::Arc          arc;
  vector<ListDigraph::Node> stack;
};

void
CFG::print_state (struct state state)
{
  ListDigraph::Arc st_arc = state.arc;
  vector<ListDigraph::Node> st_stack = state.stack;
  ListDigraph::Node st_arc_src = m_graph -> source (st_arc);
  ListDigraph::Node st_arc_trg = m_graph -> target (st_arc);
  BB *bb_src = (*m_bbs)[st_arc_src];
  BB *bb_trg = (*m_bbs)[st_arc_trg];
  
  cout << "(" << bb_src -> m_label << " -> " << bb_trg -> m_label << ") [ ";
  
  vector<ListDigraph::Node>::iterator st_ret_it = state.stack.begin ();
  for (; st_ret_it != state.stack.end (); ++st_ret_it)
    {
      ListDigraph::Node st_ret = *st_ret_it;
      BB *bb_ret = (*m_bbs)[st_ret];
      cout << bb_ret -> m_label << " ";
    }
  cout << "]" << endl;
}



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
  
  //**/BB *trg_bb = (*m_bbs)[trg];
  //**/BB *ret_bb = (*m_bbs)[ret];
  //**/cout << endl;
  //**/cout << bb -> m_label << endl;
  //**/cout << " push  : " << ret_bb -> m_label << endl;
  //**/cout << " next  : " << trg_bb -> m_label << endl;

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

      //**/cout << endl;
      /*
      vector<struct state>::iterator reached_it = reached.begin ();
      for (; reached_it != reached.end (); ++reached_it)
	{
	  struct state reached = *reached_it;
	  print_state (reached);
	}
      */
      //**/BB *src_bb = (*m_bbs)[src];
      BB *trg_bb = (*m_bbs)[trg];
      Inst *last_inst = trg_bb -> m_insts -> back ();
      //**/cout << trg_bb -> m_label << " (from " << src_bb -> m_label << ")" << endl;

      // copy stack from state:
      //**/cout << " stack :";
      vector<ListDigraph::Node>::iterator ret_it = state.stack.begin ();
      for (; ret_it != state.stack.end (); ++ret_it)
	{
	  ListDigraph::Node ret = *ret_it;
	  stack_t.push_back (ret);
	  
	  //**/BB *ret_bb = (*m_bbs)[ret];
	  //**/cout << " " << ret_bb -> m_label;
	}
      //**/cout << endl;

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
	  //**/cout << " pop   : " << ret_bb -> m_label << endl;
	  
	  bool exists_arc = false;
	  ListDigraph::OutArcIt arc (*m_graph, trg);
	  for (; arc != INVALID; ++arc)
	    if (m_graph -> target (arc) == ret)
	      exists_arc = true;
	  
	  if (!exists_arc)
	    {
	      addEdge (*trg_bb, *ret_bb);
	      //**/cout << " add   : " << trg_bb -> m_label << " -> "  << ret_bb -> m_label << endl;
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
	  
	  //**/cout << " next  :";	  
	  //**/BB *ubb = (*m_bbs)[ret];
	  //**/cout << " " << ubb -> m_label;

	  if (!last_inst -> m_uncond)
	    {
	      //**/cout << hex << last_inst -> m_addr << ": " << last_inst -> m_disass << endl;
	      
	      ListDigraph::Node target = (*m_nodes)[last_inst -> m_target];
	      BB *target_bb = (*m_bbs)[target];
	      //**/cout << " target: " << target_bb -> m_label << endl;
	      
	      bool exists_arc = false;
	      ListDigraph::OutArcIt arc (*m_graph, trg);
	      for (; arc != INVALID; ++arc)
		if (m_graph -> target (arc) == target)
		  exists_arc = true;
	      
	      if (!exists_arc)
		{
		  addEdge (*trg_bb, *target_bb);
		  //**/cout << " add   : " << trg_bb -> m_label << " -> "  << target_bb -> m_label << endl;
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

	  (*m_preds)[ret] -> push_back (trg_bb);
	  (*m_succs)[trg] -> push_back (ret_bb);
	  
	  // TODO: hacky ; to clean
	  //last_inst -> m_disass = "blr";
	  last_inst -> m_refs   = 0x80; // sets pc
	  last_inst -> m_defs   = 0x80; // id.
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
	      
	      //**/BB *ret_bb = (*m_bbs)[ret];
	      //**/cout << " push  : " << ret_bb -> m_label << endl;
	    } 

	  // add states too w if not yet reached:
	  ListDigraph::Arc arc;
	  m_graph -> firstOut (arc, trg);
	  state.stack = stack_t;
	  //**/cout << " next  :";
	  while (arc != INVALID)
	    {
	      ListDigraph::Node u = m_graph -> target (arc);
	      state.arc = arc;
	      //**/cout << endl;
	      //**/print_state (state);
	      //**/cout << "---" << endl;
	      
	      bool found = false;
	      vector<struct state>::iterator reach_it = reached.begin ();
	      for (; reach_it != reached.end (); ++reach_it)
		{
		  struct state reach = *reach_it;
		  //**/print_state (reach);
	      
		  if (state.arc == reach.arc)
		    {
		      //**/cout << "same arc" << endl;
		      if (state.stack.size () == reach.stack.size ())
			{
			  //**/cout << "same stack size" << endl;
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
		{
		  w.push_back (state);
		  
		  //**/BB *ubb = (*m_bbs)[u];
		  //**/cout << " " << ubb -> m_label;
		}
	      
	      m_graph -> nextOut (arc);
	    }
	}
      //**/cout << endl;
    }
}
