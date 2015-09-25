#include "arch.h"
#include "codeReader.h"
#include "staticInfo.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      cerr << "Usage: " << argv[0] << " FILE" << endl;
      return EXIT_FAILURE;
    }

  arch a;
  a.readCodeFile (argv[1]);
  u32 address = a.programCounter ();
  cout << "start " << address << endl;

  vector<staticInfo *> infos;
  codeReader *reader = a.getCodeReader ();
  unsigned int const nbCodeSection = reader -> getNbCodeSection ();
  for (unsigned int i = 0; i < nbCodeSection; ++i)
    {
      codeSection *section = reader -> getCodeSection (i);
      address = section -> v_addr ();
      u32 addressEnd = address + section -> size ();
      while (address < addressEnd)
	{
	  staticInfo *info = a.getInstructionStaticInfo (address);
	  infos.push_back (info);
	}
    }

  return 0;
}
