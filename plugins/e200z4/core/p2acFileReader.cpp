#include "arch.h"
#include "codeReader.h"
#include <stdlib.h>

using namespace std;

class codeReader;

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      cerr << "Usage ./p2aFileReader code_file" << endl;
      exit (EXIT_FAILURE);
    }

  arch a;
  a.readCodeFile (argv[1]);
  u32 address = a.programCounter ();
  cout << "start " << address << endl;
  
  codeReader *reader = a.getCodeReader ();
  unsigned int const nbCodeSection = reader -> getNbCodeSection ();
  for (int i = 0; i < nbCodeSection; ++i)
    {
      codeSection *section = reader -> getCodeSection (i);
      address = section -> v_addr ();
      u32 addressEnd = address + section -> size ();
      while (address < addressEnd)
	{
	  string info = a.getInstructionStaticInfo (address);
	  cout << info << endl;
	}
    }
  
  return 0;
}
