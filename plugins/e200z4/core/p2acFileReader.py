#!/usr/bin/python
# vim: set fileencoding=utf-8 :
#
# Read an application file (.elf) and:
#  * decode binary part
#  * export static instruction information: register read/write, branch inst and so on.

# usage ./p2aFileReader <code_file>

import sys
import os
from e200z4 import arch

if len(sys.argv) != 2:
	print "Usage ./p2aFileReader code_file"
	os._exit(1)

f=arch()
f.readCodeFile(sys.argv[1])
print "start %d" % f.programCounter()
address=f.programCounter()
for i in range(f.getCodeReader().getNbCodeSection()):
	section = f.getCodeReader().getCodeSection(i);
	address = section.v_addr();
	addressEnd = address + section.size();
	while(address < addressEnd):
		info,address=f.getInstructionStaticInfo(address)
		print info

