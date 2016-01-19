#include "PDG.hpp"

#include "CFG.hpp"
#include "CDG.hpp"
#include "DDG.hpp"
#include <sstream>

using namespace std;
using namespace lemon;

PDG::PDG (CFG &cfg, CDG &cdg, DDG &ddg)
{
  ostringstream ss_name, ss_label;
  ss_name  << "pdg";
  ss_label << "PDG(" << cfg.m_label << ")";

  m_name  = ss_label.str ();
  m_label = ss_label.str ();
}
