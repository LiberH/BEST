#include "PDT.hpp"

#include "CFG.hpp"
#include <sstream>

using namespace std;

/* PUBLIC */

PDT::PDT (const CFG *cfg) :
  DT (CFG::Reverse (cfg))
{
  ostringstream ss_name, ss_label;
  ss_name  << "pdt";
  ss_label << "PDT(" << cfg -> m_label << ")";

  m_name  = ss_name.str ();
  m_label = ss_label.str ();
}
