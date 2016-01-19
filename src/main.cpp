#include "CFG.hpp"
#include "PDT.hpp"
#include "CDG.hpp"
#include "DDG.hpp"
#include "PDG.hpp"
#include "Dot.hpp"

using namespace std;

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      cerr << "Usage: " << argv[0] << " FILE" << endl;
      return EXIT_FAILURE;
    }

  CFG *cfg  = CFG::FromFile (argv[1]);
  PDT *pdt  = new PDT (*cfg);
  CDG *cdg  = new CDG (cfg, pdt);
  DDG *ddg  = new DDG (*cfg);
  PDG *pdg  = new PDG (*cfg, *cdg, *ddg);
  
  Dot::toFile ("test/cfg.dot", *cfg);
  Dot::toFile ("test/pdt.dot", *pdt);
  Dot::toFile ("test/cdg.dot", *cdg);
  Dot::toFile ("test/ddg.dot", *cfg, *ddg);
  Dot::toFile ("test/pdg.dot", *cfg, *pdg);

  return EXIT_SUCCESS;
}
