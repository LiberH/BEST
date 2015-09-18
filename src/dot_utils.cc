#include <graphviz/cgraph.h>
#include <iostream>

using namespace std;

static int putstr (void *, const char *);
static int flush (void *);
static Agiodisc_t agiodisc = { AgIoDisc.afread, &putstr, &flush };
Agdisc_t agdisc = { NULL, NULL, &agiodisc };

static int
putstr (void *chan, const char *str)
{
  (*((ostream *) chan)) << str;
  return 0;
}

static int
flush (void *chan)
{
  (*((ostream *) chan)).flush ();
  return 0;
}
