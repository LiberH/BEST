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

  vector<Inst *> *insts = Inst::FromFile (argv[1]);
  CFG            *cfg   =  CFG::FromFile (argv[1]);
  vector<BB   *> *bbs   = cfg -> bbs ();
         BB      *bb0   = bbs -> back ();

  CFG *gfc = CFG::Reverse (cfg);
  DFS *dfs = new DFS (gfc);
  PDT *pdt = new PDT (cfg);
  CDG *cdg = new CDG (cfg , pdt);
  DDG *ddg = new DDG (cfg);
  PDG *pdg = new PDG (cfg, cdg , ddg);
  
  Inst::ToFile ("test/insts.dmp" , insts      );
    BB::ToFile ("test/bbs.dmp"   , bbs        );
    BB::ToFile ("test/bb0.dot"   , bb0        );
   CFG::ToFile ("test/cfg.dot"   , cfg        );
   DFS::ToFile ("test/dfs.dot"   , gfc   , dfs);
   PDT::ToFile ("test/pdt.dot"   , pdt        );
   CDG::ToFile ("test/cdg.dot"   , cdg        );
   DDG::ToFile ("test/ddg.dot"   , cfg   , ddg);
   PDG::ToFile ("test/pdg.dot"   , pdg        );

  Slicer *slicer = new Slicer (cfg, pdg);
  vector<Inst *> *slice = slicer -> slice (0x3044);
  Inst::ToFile ("test/slice.dmp" , slice);
   
  return EXIT_SUCCESS;
}
