#include "Inst.hpp"
#include "BB.hpp"
#include "CFG.hpp"
#include "DFS.hpp"
#include "PDT.hpp"
#include "CDG.hpp"
#include "DDG.hpp"
#include "PDG.hpp"
#include "Slicer.hpp"

#include <vector>
#include <ctime>

using namespace std;

int
main (int argc, char *argv[])
{
  string template_path   = "";
  string executable_path = "";
  string option          = "";
  bool   print_usage     = false;
  bool   cfg_only        = false;

  // TODO: getopt
  switch (argc)
    {
    case 3:
      template_path   = argv[1];
      template_path   = template_path.substr (template_path.find_last_of ("=") +1);
      executable_path = argv[2];
      break;
      
    case 4:
      template_path   = argv[1];
      template_path   = template_path.substr (template_path.find_last_of ("=") +1);
      executable_path = argv[2];
      option          = argv[3]; cfg_only = true;
      break;
      
    default:
      print_usage = true;
      break;
    }

  if (print_usage)
    {
      cerr << "Usage: " << argv[0] << " --template=TEMPLATE_FILE EXECUTABLE_FILE [--cfg-only]" << endl;
      return EXIT_FAILURE;      
    }
  
  string filename = executable_path.substr (executable_path.find_last_of ("/") +1);
  string basename = filename.substr (0, filename.find_last_of ("."));
  string name     = basename.substr (0, basename.find_last_of ("-"));
  string optim    = basename.substr (basename.find_last_of ("-") +1);
    
  if (cfg_only)
    {
      CFG *cfg = CFG::FromFile (executable_path);
      CFG::ToFile (executable_path + "-cfg.dot" , cfg);

      return EXIT_SUCCESS;
    }

  clock_t begin = clock ();
      
  ////////////////////
  CFG *cfg = CFG::FromFile (executable_path);
  vector<Inst *> *dump = cfg -> insts ();
  vector<BB   *> *bbs  = cfg -> bbs   ();
  
  CFG *gfc = CFG::Reverse (cfg);
  DFS *dfs = new DFS (gfc);
  PDT *pdt = new PDT (cfg);
  CDG *cdg = new CDG (cfg , pdt);
  DDG *ddg = new DDG (cfg);
  PDG *pdg = new PDG (cfg, cdg , ddg);
  
  Slicer *slicer = new Slicer (cfg, pdg);
  vector<Inst *> *slice = slicer -> slice ();
  
  int dump_regs  = Inst::CountRegs (dump);
  int slice_regs = Inst::CountRegs (slice);
  ////////////////////
  
  clock_t end = clock ();
  double time = double (end - begin) / CLOCKS_PER_SEC;
  
  cout << name       << ","
       << optim      << ","
       << dump_regs  << ","
       << slice_regs << ","
       << time       << endl;
  
  Inst::ToFile (executable_path + "-dump"    , dump);
  Inst::ToFile (executable_path + "-slice"   , slice);
    BB::ToFile (executable_path + "-bbs"     , bbs);
   CFG::ToFile (executable_path + "-cfg.dot" , cfg);
   
   DFS::ToFile (executable_path + "-dfs.dot" , gfc, dfs);
    DT::ToFile (executable_path + "-dt.dot"  , pdt);
   PDT::ToFile (executable_path + "-pdt.dot" , pdt);
   CDG::ToFile (executable_path + "-cdg.dot" , cdg);
   PDG::ToFile (executable_path + "-pdg.dot" , pdg);

   CFG::ToUPPAAL (executable_path + "-model.xml", template_path, cfg, slice);
  
  return EXIT_SUCCESS;
}

/*
int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      cerr << "Usage: " << argv[0] << " FILE" << endl;
      return EXIT_FAILURE;
    }
  
  string filename = string (argv[1]);
  string basename = filename.substr (0, filename.find_last_of(".")); 
  string prefix   = basename + "-";;

  // TODO: build "insts" from "cfg"
  CFG            *cfg   = CFG::FromFile (filename);
  vector<BB   *> *bbs   = cfg -> bbs   ();
  vector<Inst *> *insts = cfg -> insts ();
  
   CFG::ToFile (prefix + "cfg.dot"   , cfg        );
    BB::ToFile (prefix + "bbs.dmp"   , bbs        );
  Inst::ToFile (prefix + "insts.dmp" , insts      );

  CFG *gfc = CFG::Reverse (cfg);
  DFS *dfs = new DFS (gfc);
  PDT *pdt = new PDT (cfg);
  CDG *cdg = new CDG (cfg , pdt);
  DDG *ddg = new DDG (cfg);
  PDG *pdg = new PDG (cfg, cdg , ddg);
  
   DFS::ToFile (prefix + "dfs.dot"   , gfc   , dfs);
   PDT::ToFile (prefix + "pdt.dot"   , pdt        );
   CDG::ToFile (prefix + "cdg.dot"   , cdg        );
   DDG::ToFile (prefix + "dds.dmp"   , insts , ddg);
   DDG::ToFile (prefix + "ddg.dot"   , cfg   , ddg);
   PDG::ToFile (prefix + "pdg.dot"   , pdg        );

  Slicer *slicer = new Slicer (cfg, pdg);
  vector<Inst *> *slice = slicer -> slice ();
  vector<Inst *> *mini  = Slicer::MinimizeSlice (slice);
  Inst::ToFile   (prefix + "slice.dmp" , slice);
// CFG::ToUPPAAL (prefix + "model.xml" , cfg, mini);
  
  int insts_count = Inst::CountRegs (insts);
  int mini_count  = Inst::CountRegs (mini);
  cerr << "Registers use: " << mini_count << "/" << insts_count << endl;

  return EXIT_SUCCESS;
}
*/
