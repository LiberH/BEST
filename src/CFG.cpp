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
CFG::ToFile (string fn, CFG *cfg)
{
  FILE *f = fopen (C(fn), "w");
  Agraph_t *agraph = agopen (C(cfg -> m_label), Agdirected, NULL);
  agattr (agraph, AGRAPH, "label", C(cfg -> m_label));
  agattr (agraph, AGEDGE, "style", "solid");

  ListDigraph *graph = cfg -> m_graph;
  ListDigraph::NodeMap<Agnode_t *> *agnodes = new ListDigraph::NodeMap<Agnode_t *> (*graph);

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
  
  agwrite (agraph, f);
  agclose (agraph);
  fclose (f);
}

static
string reg_names[] = {
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
CFG::ToUPPAAL (string fn, CFG *cfg, vector<Inst *> *slice)
{
  ListDigraph::NodeMap<struct pos> pz (*cfg -> m_graph);
  {
    FILE *f = fopen ("bench/bin-gcc/fibcall-O1.elf-cfg-extra.dot", "r");
    Agraph_t* g = agread (f, NULL);

    Agnode_t *n = agfstnode(g);
    for (; n != NULL; n = agnxtnode (g, n))
      {
	char *label = agget (n, "label");
	char *pos   = agget (n, "pos");
	pos = (pos ? pos : (char *) "null");

	string sx, sy;
	stringstream ss (pos);
	getline (ss, sx, ',');
	getline (ss, sy, ',');
	
	ListDigraph::Node node = cfg -> findByLabel (label);
	stringstream ssx (sx);
	stringstream ssy (sy);
	ssx >> pz[node].x; pz[node].x *= 2; pz[node].x *= -1;
	ssy >> pz[node].y; pz[node].y *= 2; 
      }
    
    agclose (g);
    fclose (f);
  }

  //////////////

  ostringstream oss;
  XMLDocument *doc = new XMLDocument ();
  doc -> LoadFile ("template.xml");

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
  bitset<64> bs (refs);
  for (int b = 0; b < 64; ++b)
    if (b != 7 && bs[b])
      oss << " " << reg_names[b] << ",";
  oss.seekp (-1, oss.cur);
  oss << ";" << endl;

  int n_insts = insts -> size ();
  oss << "const int N_INSTS = " << n_insts << ";" << endl;
  oss << nta_decl_txt << endl;
  oss << "const inst_t insts[N_INSTS] = {";
  
  ListDigraph::NodeIt o (*cfg -> m_graph);
  for (; o != INVALID; ++o)
    {
      BB *bb = (*cfg -> m_bbs)[o];
      string label, spaces;
      ostringstream o;
      o.str ("");
      o << bb -> m_label;
      label = o.str ();
      spaces = string (23 - label.length (), ' ');

      oss << endl;
      oss << endl << "  /* " << label << spaces << " */";
       
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it = insts -> begin ();
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  string disass, spaces;
	  ostringstream o;
	  o.str ("");
	  o << hex << inst -> m_addr << ": " << inst -> m_disass;
	  disass = o.str ();
	  spaces = string (22 - disass.length (), ' ');
	  
	  oss << endl;
	  oss << "  /*  " << disass << spaces << " */ { ";
	  oss << dec << inst -> m_addr                    << ", ";
	  oss << 1                                        << ", "; // latency;
	  oss << (inst -> m_branch ? "true, " : "false,") << " ";
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

  oss.seekp (-1, oss.cur);
  oss << endl << "};" << endl;
  oss << endl;

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

  // <location id="idinit">
  Inst *entry = (*cfg -> m_bbs)[cfg -> m_entry] -> m_insts -> front ();
  ListDigraph::Node entry_n = (*cfg -> m_nodes)[entry -> m_addr];
  XMLElement *tmplt_loc_init = doc -> NewElement ("location");	  
  XMLElement *tmplt_urg = doc -> NewElement ("urgent");	  
  tmplt_loc_init -> SetAttribute ("id", "idinit");
  tmplt_loc_init -> SetAttribute ("x", pz[entry_n].x);
  tmplt_loc_init -> SetAttribute ("y", -pz[entry_n].y);
  tmplt_loc_init -> InsertEndChild (tmplt_urg);
  tmplt -> InsertEndChild (tmplt_loc_init);

  // <location id="idexit">
  Inst *exit = (*cfg -> m_bbs)[cfg -> m_exit] -> m_insts -> back ();
  ListDigraph::Node exit_n = (*cfg -> m_nodes)[exit -> m_addr];
  XMLElement *tmplt_loc_exit = doc -> NewElement ("location");	  
  tmplt_loc_exit -> SetAttribute ("id", "idexit");
  tmplt_loc_exit -> SetAttribute ("x", pz[exit_n].x);
  tmplt_loc_exit -> SetAttribute ("y", -pz[exit_n].y);
  tmplt -> InsertEndChild (tmplt_loc_exit);

  // <location id="id...">
  ListDigraph::NodeIt n (*cfg -> m_graph);
  for (; n != INVALID; ++n)
    {
      BB *bb = (*cfg -> m_bbs)[n];
      vector<Inst *> *insts = bb -> m_insts;
      vector<Inst *>::iterator inst_it = insts -> begin ();  
      for (; inst_it != insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  
	  XMLElement *tmplt_loc = doc -> NewElement ("location");	  
	  oss.str ("");
	  oss << "id" << hex << inst -> m_addr;
	  tmplt_loc -> SetAttribute ("id", C(oss.str ()));
	  tmplt_loc -> SetAttribute ("x", pz[n].x);
	  tmplt_loc -> SetAttribute ("y", -pz[n].y);
	  tmplt -> InsertEndChild (tmplt_loc);

	  if (inst == bb -> m_insts -> front ())
	    {
	      XMLElement *tmplt_loc_name = doc -> NewElement ("name");
	      oss.str ("");
	      oss << bb -> m_label;
	      //oss << "_" << hex << inst -> m_addr;
	      tmplt_loc_name -> SetText (C(oss.str ()));
	      tmplt_loc -> InsertEndChild (tmplt_loc_name);
	    }
	}
    }

  // <init ref="idinit">
  XMLElement *tmplt_init = doc -> NewElement ("init");	  
  tmplt_init -> SetAttribute ("ref", "idinit");
  tmplt -> InsertEndChild (tmplt_init);


  // First transition:
  XMLElement *tmplt_ftr = doc -> NewElement ("transition");
  XMLElement *tmplt_ftr_src = doc -> NewElement ("source");
  tmplt_ftr_src -> SetAttribute ("ref", "idinit");
  tmplt_ftr -> InsertEndChild (tmplt_ftr_src);
  
  XMLElement *tmplt_ftr_trg = doc -> NewElement ("target");
  oss.str ("");
  oss << "id" << hex << entry -> m_addr;
  tmplt_ftr_trg -> SetAttribute ("ref", C(oss.str ()));
  tmplt_ftr -> InsertEndChild (tmplt_ftr_trg);
  
  XMLElement *tmplt_ftr_label0 = doc -> NewElement ("label");
  tmplt_ftr_label0 -> SetAttribute ("kind", "synchronisation");
  tmplt_ftr_label0 -> SetText ("initialize!");
  tmplt_ftr -> InsertEndChild (tmplt_ftr_label0);

  XMLElement *tmplt_ftr_label1 = doc -> NewElement ("label");
  oss.str ("");
  oss << "t = 0,\n";
  oss << "jmp(" << dec << entry -> m_addr << ")\n";
  tmplt_ftr_label1 -> SetAttribute ("kind", "assignment");
  tmplt_ftr_label1 -> SetText (C(oss.str ()));
  tmplt_ftr -> InsertEndChild (tmplt_ftr_label1);

  tmplt -> InsertEndChild (tmplt_ftr);

  // Last transition:
  XMLElement *tmplt_ltr = doc -> NewElement ("transition");
  XMLElement *tmplt_ltr_src = doc -> NewElement ("source");
  oss.str ("");
  oss << "id" << hex << exit -> m_addr;
  tmplt_ltr_src -> SetAttribute ("ref", C(oss.str ()));
  tmplt_ltr -> InsertEndChild (tmplt_ltr_src);
  
  XMLElement *tmplt_ltr_trg = doc -> NewElement ("target");
  tmplt_ltr_trg -> SetAttribute ("ref", "idexit");
  tmplt_ltr -> InsertEndChild (tmplt_ltr_trg);
  
  XMLElement *tmplt_ltr_label0 = doc -> NewElement ("label");
  tmplt_ltr_label0 -> SetAttribute ("kind", "synchronisation");
  tmplt_ltr_label0 -> SetText ("stop[F]!");
  tmplt_ltr -> InsertEndChild (tmplt_ltr_label0);
    
  tmplt -> InsertEndChild (tmplt_ltr);

  // Other transitions:
  ListDigraph::NodeIt m (*cfg -> m_graph);
  for (; m != INVALID; ++m)
    {
      BB *bb = (*cfg -> m_bbs)[m];
      Inst *prev = NULL;
      vector<Inst *>::iterator inst_it = bb -> m_insts -> begin ();  
      for (; inst_it != bb -> m_insts -> end (); ++inst_it)
	{
	  Inst *inst = *inst_it;
	  
	  if (prev)
	    {
	      XMLElement *tmplt_tr = doc -> NewElement ("transition");
	      XMLElement *tmplt_tr_src = doc -> NewElement ("source");
	      oss.str ("");
	      oss << "id" << hex << prev -> m_addr;
	      tmplt_tr_src -> SetAttribute ("ref", C(oss.str ()));
	      tmplt_tr -> InsertEndChild (tmplt_tr_src);
	      
	      XMLElement *tmplt_tr_trg = doc -> NewElement ("target");
	      oss.str ("");
	      oss << "id" << hex << inst -> m_addr;
	      tmplt_tr_trg -> SetAttribute ("ref", C(oss.str ()));
	      tmplt_tr -> InsertEndChild (tmplt_tr_trg);
	      
	      XMLElement *tmplt_tr_label0 = doc -> NewElement ("label");
	      tmplt_tr_label0 -> SetAttribute ("kind", "synchronisation");
	      tmplt_tr_label0 -> SetText ("fetch!");
	      tmplt_tr -> InsertEndChild (tmplt_tr_label0);
	      
	      XMLElement *tmplt_tr_label1 = doc -> NewElement ("label");
	      tmplt_tr_label1 -> SetAttribute ("kind", "assignment");
	      if (find (slice -> begin (), slice -> end (), prev ) != slice -> end ())
		{
		  oss.str ("");
		  oss << "update(),\n";
		  oss << "execute_" << hex << prev -> m_addr << "()";
		  tmplt_tr_label1 -> SetText (C(oss.str ()));
		}
	      else
		{
		  oss.str ("");
		  oss << "update()";
		  tmplt_tr_label1 -> SetText (C(oss.str ()));
		}
	      
	      tmplt_tr -> InsertEndChild (tmplt_tr_label1);
	      tmplt -> InsertEndChild (tmplt_tr);
	    }
	  
	  prev = inst;
	}

      Inst *last = bb -> m_insts -> back ();
      vector<BB *> *succs = (*cfg -> m_succs)[m];
      vector<BB *>::iterator succ_it = succs -> begin ();
      for (; succ_it != succs -> end (); ++succ_it)
	{
	  BB *succ = *succ_it;
	  Inst *inst = succ -> m_insts -> front ();

	  // remove last self loop
	  if (last -> m_next == NULL)
	    continue;
	  
	  XMLElement *tmplt_tr = doc -> NewElement ("transition");
	  XMLElement *tmplt_tr_src = doc -> NewElement ("source");
	  oss.str ("");
	  oss << "id" << hex << last -> m_addr;
	  tmplt_tr_src -> SetAttribute ("ref", C(oss.str ()));
	  tmplt_tr -> InsertEndChild (tmplt_tr_src);
	  
	  XMLElement *tmplt_tr_trg = doc -> NewElement ("target");
	  oss.str ("");
	  oss << "id" << hex << inst -> m_addr;
	  tmplt_tr_trg -> SetAttribute ("ref", C(oss.str ()));
	  tmplt_tr -> InsertEndChild (tmplt_tr_trg);

	  XMLElement *tmplt_tr_label_ = doc -> NewElement ("label");
	  tmplt_tr_label_ -> SetAttribute ("kind", "guard");
	  if (last -> m_branch
	  &&  last -> m_test   != 0)
	    {
	      oss.str ("");
	      switch (last -> m_test)
		{
		case  1: oss <<  "lt()"; break;
		case  2: oss <<  "gt()"; break;
		case  3: oss <<  "eq()"; break;
		case  4: oss <<  "so()"; break;
		  
		case  5: oss <<  "ge()"; break;
		case  6: oss <<  "le()"; break;
		case  7: oss <<  "ne()"; break;
		case  8: oss <<  "--()"; break;
		  
		case  9: oss <<   "z()"; break;
		case 10: oss <<  "nz()"; break;
		default: oss << "false"; break;
		}		  
	      
	      if (inst -> m_addr != last -> m_addr +4)
		tmplt_tr_label_ -> SetText (C(oss.str ()));
	      else
		tmplt_tr_label_ -> SetText (C("!"+oss.str ()));
	    }
	  
	  tmplt_tr -> InsertEndChild (tmplt_tr_label_);
	  
	  XMLElement *tmplt_tr_label0 = doc -> NewElement ("label");
	  tmplt_tr_label0 -> SetAttribute ("kind", "synchronisation");
	  tmplt_tr_label0 -> SetText ("fetch!");
	  tmplt_tr -> InsertEndChild (tmplt_tr_label0);

	  XMLElement *tmplt_tr_label1 = doc -> NewElement ("label");
	  tmplt_tr_label1 -> SetAttribute ("kind", "assignment");
	  if (find (slice -> begin (), slice -> end (), last) != slice -> end ())
	    {
	      oss.str ("");
	      oss << "update(),\n";
	      oss << "execute_" << hex << last -> m_addr << "(),\n";
	      oss << "jmp(" << dec << inst -> m_addr << ")" << endl;
	      tmplt_tr_label1 -> SetText (C(oss.str ()));
	    }
	  else
	    {
	      oss.str ("");
	      oss << "update(),\n";
	      oss << "jmp(" << dec << inst -> m_addr << ")" << endl;
	      tmplt_tr_label1 -> SetText (C(oss.str ()));
	    }
	  
	  tmplt_tr -> InsertEndChild (tmplt_tr_label1);
	  tmplt -> InsertEndChild (tmplt_tr);	  
	}
    }
  nta -> InsertFirstChild (tmplt);
  nta -> InsertFirstChild (nta_decl);
  // </template>
	  
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
