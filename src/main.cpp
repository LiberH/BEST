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

using namespace std;

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
  
  vector<Inst *> *insts = Inst::FromFile (filename);
  CFG            *cfg   =  CFG::FromFile (filename);
  vector<BB   *> *bbs   = cfg -> bbs ();
         BB      *bb0   = bbs -> back ();

  CFG *gfc = CFG::Reverse (cfg);
  DFS *dfs = new DFS (gfc);
  PDT *pdt = new PDT (cfg);
  CDG *cdg = new CDG (cfg , pdt);
  DDG *ddg = new DDG (cfg);
  PDG *pdg = new PDG (cfg, cdg , ddg);
  
  Inst::ToFile (prefix + "insts.dmp" , insts      );
    BB::ToFile (prefix + "bbs.dmp"   , bbs        );
    BB::ToFile (prefix + "bb0.dot"   , bb0        );
   CFG::ToFile (prefix + "cfg.dot"   , cfg        );
   DFS::ToFile (prefix + "dfs.dot"   , gfc   , dfs);
   PDT::ToFile (prefix + "pdt.dot"   , pdt        );
   CDG::ToFile (prefix + "cdg.dot"   , cdg        );
   DDG::ToFile (prefix + "dds.dmp"   , insts , ddg);
   DDG::ToFile (prefix + "ddg.dot"   , cfg   , ddg);
   PDG::ToFile (prefix + "pdg.dot"   , pdg        );

  Slicer *slicer = new Slicer (cfg, pdg);
  vector<Inst *> *slice = slicer -> slice ();
  Inst::ToFile (prefix + "slice.dmp" , slice);
  CFG::ToUPPAAL (prefix + "model.xml", cfg, slice);
  
  int insts_count = Inst::CountRegs (insts);
  int slice_count = Inst::CountRegs (slice);
  cout << "Registers use: " << slice_count << "/" << insts_count << endl;
  
  return EXIT_SUCCESS;
}
