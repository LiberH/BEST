#ifndef _STATICINFO_H_
#define _STATICINFO_H_

#include "types.h"

class staticInfo {
public:
  u32 pc;
  std::string mnemo;
  u64 write_regs;
  u64 read_regs;
  bool is_branch;
  bool is_unkown;
  bool do_link;
  bool is_uncond;
  u64 target;
  
  staticInfo ();
};

#endif // _STATICINFO_H_
