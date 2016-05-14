# crt0.S

# Startup file
# initialize the stack Pointer with information provided by the linker
# Should be compiled (with 'as') and linked with any program aimed to run on Harmless PowerPC
# simulator

# powerpc-elf-as crt0.S -o crt0.o
.text
.globl launchTest
launchTest:
	lis 1,_init_sp@h
	ori 1,1,_init_sp@l
	bl main
shouldNotHappen:
	b shouldNotHappen
