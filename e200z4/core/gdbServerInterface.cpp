/****************************************************************************
 * GDB stub for Harmless
 *
 *  Original author:      Glenn Engel 
 *
 *************
 *
 *    The following gdb commands are supported:
 *
 * command          function                               Return value
 *
 *    g             return the value of the CPU registers  hex data or ENN
 *    G             set the value of the CPU registers     OK or ENN
 *
 *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
 *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
 *
 *    c             Resume at current address              SNN   ( signal NN)
 *    cAA..AA       Continue at address AA..AA             SNN
 *
 *    s             Step one instruction                   SNN
 *    sAA..AA       Step one instruction from AA..AA       SNN
 *
 *    k             kill
 *
 *    ?             What was the last sigval ?             SNN   (signal NN)
 *
 * All commands and responses are sent with a packet which includes a
 * checksum.  A packet consists of
 *
 * <packet info>#<checksum>.
 *
 * where
 * <packet info> :: <characters representing the command or response>
 * <checksum>    :: < two hex digits computed as modulo 256 sum of <packetinfo>>
 *
 * When a packet is received, it is first acknowledged with either '+' or '-'.
 * '+' indicates a successful transfer.  '-' indicates a failed transfer.
 *
 * Example:
 *
 * Host:                  Reply:
 * $m0,10#2a               +$00010203040506070809101112131415#42
 *
 ****************************************************************************/

#include <string.h>
#include <signal.h>
#include <stdio.h>

#include "arch.h"

/************************************************************************
 *
 * external low-level support routines
 */

extern int putDebugChar(int c);	/* write a single character      */
extern int getDebugChar(unsigned char *c);	/* read and return a single char */

/************************************************************************/

/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/

/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048	/*TODO déterminer cette taille en fonction du nombre, taille des registres */

static const char hexchars[] = "0123456789abcdef";

/* Convert ch from a hex digit to an int */

static int hex(unsigned char ch)
{
	if(ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9')
		return ch - '0';
	if(ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return -1;
}

static char remcomInBuffer[BUFMAX];	/* Remote communication Input Buffer */
static char remcomOutBuffer[BUFMAX];	/* Remote communication Output Buffer */

/* scan for the sequence $<data>#<checksum>     */
char *getpacket(void)
{
	char *buffer = &remcomInBuffer[0];
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	unsigned char ch = 0;

	while(1)
	{
		/* wait around for the start character, ignore all other characters */
		do
		{
			if(getDebugChar(&ch))
				return NULL;
		}
		while(ch != '$');

	  retry:
		checksum = 0;
		xmitcsum = -1;
		count = 0;

		/* now, read until a # or end of buffer is found */
		while(count < BUFMAX - 1)
		{
			if(getDebugChar(&ch))
				return NULL;
			if(ch == '$')
				goto retry;
			if(ch == '#')
				break;
			checksum = checksum + ch;
			buffer[count] = ch;
			count = count + 1;
		}
		buffer[count] = 0;

		if(ch == '#')
		{
			if(getDebugChar(&ch))
				return NULL;
			xmitcsum = hex(ch) << 4;
			if(getDebugChar(&ch))
				return NULL;
			xmitcsum += hex(ch);

			if(checksum != xmitcsum)
			{
				if(putDebugChar('-'))	/* failed checksum */
					return NULL;
			}
			else
			{
				if(putDebugChar('+'))	/* successful transfer */
					return NULL;
				/* if a sequence char is present, reply the sequence ID */
				if(buffer[2] == ':')
				{
					if(putDebugChar(buffer[0]) || putDebugChar(buffer[1]))
						return NULL;

					return &buffer[3];
				}

				return &buffer[0];
			}
		}
	}
}

/* send the packet in buffer.  */
int putpacket(char *buffer)
{
	unsigned char checksum;
	int count;
	unsigned char ch;

	/*  $<packet info>#<checksum>. */
	do
	{
		if(putDebugChar('$'))
			return 1;
		checksum = 0;
		count = 0;

		while((ch = (buffer[count])))
		{
			if(putDebugChar(ch))
				return 1;
			checksum += ch;
			count += 1;
		}

		if(putDebugChar('#') || putDebugChar(hexchars[checksum >> 4])
		  || putDebugChar(hexchars[checksum & 0xf]))
			return 1;

		if(getDebugChar(&ch))
			return 1;

	}
	while(ch != '+');

	return 0;
}

/* Convert the memory at addr address into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null).
 */

static char *mem2hex(int addr, char *buf, int count, arch & archi)
{
	unsigned char ch;
	int countInit = count;

	while(count-- > 0)
	{
		ch = archi.gdbppc_read8(addr + countInit - count - 1);
#ifdef DEBUG
		printf("%02X@%X", ch, addr + countInit - count - 1);	/* Affiche la lecture mémoire sur fond jaune */
#endif
		*buf++ = hexchars[ch >> 4];
		*buf++ = hexchars[ch & 0xf];
	}

	*buf = 0;

	return buf;
}

/* convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written */

static char *hex2mem(char *buf, int addr, int count, arch & archi)
{
	int i;
	unsigned char ch;

	for(i = 0; i < count; i++)
	{
		ch = hex(*buf++) << 4;
		ch |= hex(*buf++);
		archi.gdbppc_write8(addr+i, ch);
#ifdef DEBUG
		printf("%02X@%X", ch, addr + i);	/* Affiche l'écriture mémoire sur fond cyan */
#endif

	}

	return buf;
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */

static int hexToInt(char **ptr, int *intValue)
{
	int numChars = 0;
	int hexValue;

	*intValue = 0;

	while(**ptr)
	{
		hexValue = hex(**ptr);
		if(hexValue < 0)
			break;

		*intValue = (*intValue << 4) | hexValue;
		numChars++;

		(*ptr)++;
	}

	return (numChars);
}

void writeTPacket(char *ptr, arch & archi, int sigval)
{
	unsigned int val;
	unsigned char nbBits;
	const unsigned char nb = archi.gdbppc_getNBRegister();
	*ptr++ = 'T';
	ptr += sprintf(ptr, "%02X", sigval);
    for(unsigned int i = 0; i < nb; i++)
	{
		val = archi.gdbppc_getRegister(i, nbBits);
		ptr += sprintf(ptr, "%02X:%0*X;", i, nbBits >> 2, val); 
	}
	*ptr++ = '\0';
}

void writegRepPacket(char **ptr, arch & archi)
{
	unsigned int val;
	unsigned char nbBits;
	const unsigned char nb = archi.gdbppc_getNBRegister();
    for(unsigned int i = 0; i < nb; i++)
	{
		val = archi.gdbppc_getRegister(i, nbBits);
		*ptr += sprintf(*ptr, "%0*X", nbBits >> 2, val); 
	}
	*(*ptr++) = '\0';
}

void writeGPacketToReg(char *ptr, arch & archi)
{
	unsigned int val;
	unsigned char ch;
	unsigned char nbBits;
	const unsigned char nb = archi.gdbppc_getNBRegister();
    for(unsigned int i = 0; i < nb; i++)
	{
		//get size.
		val = archi.gdbppc_getRegister(i, nbBits);
		val = 0;
		//get value
		const unsigned char nbBytes = nbBits >> 3;
		for(unsigned j = 0; j < nbBytes; j++) //each byte.
		{
			ch = hex(*ptr++) << 4;
			ch |= hex(*ptr++);
			val |= ch << ((nbBytes-j-1) << 3);
		}
		//update simulator
		archi.gdbppc_setRegister(i, val);
	}
}

/*
 * This function does all command procesing for interfacing to gdb.
 * returns 0 with a continu command of GDB (handle_exception will be called next
 * time the simulation stop), > 0 if the simulation should continu without GDB
 * link (Detach)., <0 if the simulation should be stopped (kill).
 */

int handle_exception(arch & archi, int sigval)
{
	char *ptr;
	int addr, length;
	static int ThreadInfoFirst=0;

	ptr = remcomOutBuffer;

	writeTPacket(remcomOutBuffer, archi, sigval);

	if(putpacket(remcomOutBuffer))
		return 2;

	while(1)
	{
		remcomOutBuffer[0] = 0;

		if(!(ptr = getpacket()))
			return 2;
		switch (*ptr++)
		{
			case '?':
				remcomOutBuffer[0] = 'S';
				sprintf(&(remcomOutBuffer[1]), "%02X", sigval);
				break;

			case 'd':	/* toggle debug flag */
				break;

			case 'g':	/* return the value of the CPU registers */
				ptr = remcomOutBuffer;
				writegRepPacket(&ptr, archi);
				break;

			case 'G':	/* set the value of the CPU registers - return OK */
				writeGPacketToReg(ptr, archi);

				strcpy(remcomOutBuffer, "OK");
				break;

			case 'm':	/* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
				/* Try to read %x,%x.  */
				if(hexToInt(&ptr, &addr) && *ptr++ == ','
				  && hexToInt(&ptr, &length))
				{
					mem2hex(addr, remcomOutBuffer, length, archi);
					break;
				}
				else
					strcpy(remcomOutBuffer, "E01");	/* Pas réussi à lire la trame */
				break;

			case 'M':	/* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
				if(hexToInt(&ptr, &addr) && *ptr++ == ','
				  && hexToInt(&ptr, &length) && *ptr++ == ':')
				{
					if(hex2mem(ptr, addr, length, archi))
						strcpy(remcomOutBuffer, "OK");
					else
						strcpy(remcomOutBuffer, "E03");	/* écriture fail */
				}
				else
					strcpy(remcomOutBuffer, "E02");	/*Pas capable de décoder */
				break;

			case 'c':	/* cAA..AA    Continue at address AA..AA(optional) */
				if(hexToInt(&ptr, &addr))
				{
					archi.setProgramCounter(addr);
				}
				return 0;

			case 'C':	/* cAA..AA    Continue at address AA..AA(optional) */
				return 0;

			case 's':	/*sAA..AA Execute one instruction from AA..AA (optional) */
				if(hexToInt(&ptr, &addr))
				{
					archi.setProgramCounter(addr);
					printf("{step from addr %d}", addr);
				}

#ifdef DEBUG
				printf("%s", (archi.disassemble(archi.programCounter(), 2)).c_str());	/* print instruction to execute */
#endif
				printf("{%d}", archi.execInst(1));
				writeTPacket(remcomOutBuffer, archi, 5);

				break;
			case 'Z':
				switch (*ptr++)
				{
					case '0':
					case '1':
						if(*ptr++ == ',' && hexToInt(&ptr, &addr)
						  && *ptr++ == ',' && hexToInt(&ptr, &length))
						{
							archi.addBreakpoint(addr);
							strcpy(remcomOutBuffer, "OK");
						}
						else
							strcpy(remcomOutBuffer, "E01");	/* Pas réussi à lire la trame */
						break;

					case '2':	/* Read breakpoint */
					case '3':	/* Write breakpoint */
					case '4':	/* Acces breakpoint */
						remcomOutBuffer[0] = '\0';	/* Not suported */
						break;
				}
				break;

			case 'z':
				switch (*ptr++)
				{
					case '0':
					case '1':
						if(*ptr++ == ',' && hexToInt(&ptr, &addr)
						  && *ptr++ == ',' && hexToInt(&ptr, &length))
						{
							archi.removeBreakpoint(addr);
							strcpy(remcomOutBuffer, "OK");
						}
						else
							strcpy(remcomOutBuffer, "E01");	/* Pas réussi à lire la trame */
						break;

					case '2':	/* Read  breakpoint */
					case '3':	/* Write breakpoint */
					case '4':	/* Acces breakpoint */
						remcomOutBuffer[0] = '\0';	/* Not suported */
						break;
				}
				break;

				/* kill the program */
			case 'k':

#ifdef DEBUG
				printf("{[Stub] KILL!}");
#endif
				return -1;

			case 'r':	/* Reset */
#ifdef DEBUG
				printf("{[Stub] Reset!}");
#endif
				archi.reset();
				break;
			case 'D':
#ifdef DEBUG
				printf("{[Stub] Detach}");
#endif
				return 1;
			case 'q':	/*General query packet */
				switch (*ptr++)
				{
					case 'f':
						if(strcmp(ptr, "ThreadInfo") == 0)
						{
							ThreadInfoFirst = 1;
							strcpy(remcomOutBuffer, "m0");
						}
						break;

					case 's':
						if(strcmp(ptr, "ThreadInfo") == 0)
						{
							if(ThreadInfoFirst)
							{
								ThreadInfoFirst = 0;
								strcpy(remcomOutBuffer, "l");
							}
							else
							{
								ThreadInfoFirst = 1;
								strcpy(remcomOutBuffer, "m0");
							}
						}
						break;

					case 'C':
						strcpy(remcomOutBuffer, "QC0");
						break;
				}

		}	/* switch */

		/* reply to the request */
		if(putpacket(remcomOutBuffer))
			return 2;
	}
}
