#ifndef _STATICINFO_H_
#define _STATICINFO_H_

#include "types.h"

class staticInfo {
public:
  u32 pc;
  std::string mnemo;
  std::string funct;
  u64 write_regs;
  u64 read_regs;
  bool is_branch;
  u32  test;
  bool is_unknown;
  bool do_link;
  bool is_uncond;
  u32 target;

  bool do_memory;
  
  staticInfo ();
};

#endif // _STATICINFO_H_
