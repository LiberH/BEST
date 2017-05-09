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
#include <getopt.h>

using namespace std;

int
main (int argc, char *argv[])
{
  string output_dir      = "";
  string template_file   = "";
  string executable_file = "";
  string option          = "";
  bool   print_usage     = false;
  bool   cfg_only        = false;

  static struct option long_options[] = {
    { "help"          ,       no_argument , NULL ,  'h' },
    { "output-dir"    , required_argument , NULL ,  'o' },
    { "template-file" , required_argument , NULL ,  't' },
    { "cfg-only"      ,       no_argument , NULL ,  'c' },
    { NULL            ,                 0 , NULL ,   0  }
  };

  int opt;
  int long_index = 0;
  while (!print_usage
      && (opt = getopt_long (argc, argv,"ht:o:c", long_options, &long_index )) != -1)
    {
      switch (opt)
	{
	case 'h' : print_usage     = true;   break;
	case 't' : template_file   = optarg; break;
	case 'o' : output_dir      = optarg; break;
	case 'c' : cfg_only        = true;   break;
	default  : print_usage     = true;   break;
	}
    }

  executable_file = argv[optind];
  if (             executable_file == "") print_usage = true;  
  if (!cfg_only && template_file   == "") print_usage = true;  
  if ( cfg_only && template_file   != "") print_usage = true;  
  if (print_usage)
    {
      cerr << "usage: " << argv[0] << " --help" << endl;
      cerr << "       " << argv[0] << " [--output-dir=OUTPUT_DIR] --template-file=TEMPLATE_FILE EXECUTABLE_FILE" << endl;
      cerr << "       " << argv[0] << " [--output-dir=OUTPUT_DIR] --cfg-only EXECUTABLE_FILE" << endl;
      cerr << "         -h, --help                          prompt this help                    " << endl;
      cerr << "         -t, --template-file=TEMPLATE_FILE   use TEMPLATE_FILE as UPPAAL template" << endl;
      cerr << "         -o, --output-dir=OUTPUT_DIR         output generated files to OUTPUT_DIR" << endl;
      cerr << "         -c, --cfg-only                      generate only CFG related files     " << endl;
      return EXIT_FAILURE;      
    }
  
  string filename = executable_file.substr (executable_file.find_last_of ("/") +1);
  string basename = filename.substr (0, filename.find_last_of ("."));
  string name     = basename.substr (0, basename.find_last_of ("-"));
  string optim    = basename.substr (basename.find_last_of ("-") +1);
  string outbase  = (output_dir == "" ? executable_file : output_dir + "/" + filename);
  
  if (cfg_only)
    {
      CFG            *cfg  = CFG::FromFile (executable_file);
    //vector<Inst *> *dump = cfg -> insts ();
    //vector<BB   *> *bbs  = cfg -> bbs   ();

    //Inst::ToFile (outbase + "-dump"    , dump);
    //  BB::ToFile (outbase + "-bbs"     , bbs);  
       CFG::ToFile (outbase + "-cfg.dot" , cfg, CFG::FINE_GRAIN);
    // CFG::ToFile (outbase + "-CFG.dot" , cfg, CFG::COARSE_GRAIN);

      return EXIT_SUCCESS;
    }

  clock_t begin = clock ();
      
  ////////////////////
  CFG            *cfg  = CFG::FromFile (executable_file);
  vector<Inst *> *dump = cfg -> insts ();
  
//CFG *gfc = CFG::Reverse (cfg);
//DFS *dfs = new DFS (gfc);
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
   
 // DFS::ToFile (outbase + "-dfs.dot"  , gfc, dfs);
 //  DT::ToFile (outbase + "-dt.dot"   , pdt);
 // PDT::ToFile (outbase + "-pdt.dot"  , pdt);
 // CDG::ToFile (outbase + "-cdg.dot"  , cdg);
 // PDG::ToFile (outbase + "-pdg.dot"  , pdg);
 //Inst::ToFile (outbase + "-slice"    , slice);

   CFG::ToUPPAAL (outbase + "-model.xml"        , template_file, cfg, dump);
   CFG::ToUPPAAL (outbase + "-model_sliced.xml" , template_file, cfg, slice);
  
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
