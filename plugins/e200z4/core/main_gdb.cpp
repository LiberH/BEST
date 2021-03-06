/**
 * generated by HARMLESS : 'Hardware ARchitecture Modeling Language
 *                          for Embedded Software Simulation'
 * model : e200z4
 *
 */

#include "instructionBase.h"
#include "arch.h"
#include "instDecoder.h"
#include "utils.h"
#include <unistd.h>

#include <iostream>
#include <stdlib.h>

#include <signal.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>

using namespace std;

/* GDB socket */
int GDBsock;
bool continu;
arch *ZeArch = NULL;
int sig_pipe_received = 0;

extern int handle_exception(arch &, int);
extern void putpacket(char *buffer);

void Die(char *mess)
{
	perror(mess);
	exit(EXIT_FAILURE);
}

void print_usage(const char *prog_name)
{
	fprintf(stderr,
	  "Usage: %s -f code_file [-p port]\n"
	  "\t-f : Code file to execute\n\t-p : Port to connect GDB\n", prog_name);
}

/* Handle du signal SIGUSR1 */
void sig_usr1(int sigval)
{
	printf("SIGUSR!\n");
	if(ZeArch != NULL)
		ZeArch->stopSimu(18);
}

/* Handle du signal SIGTERM */
void sig_term(int sigval)
{
	printf("SIGTERM!\n");
	continu = false;
	if(ZeArch != NULL)
		ZeArch->stopSimu(18); /* Stop la simulation */
	fflush(stdout);
}

/* Handle du signal SIGPIPE */
void sig_pipe(int sigval)
{
	fprintf(stderr, "SIGPIPE received \n");
	sig_pipe_received = 1;
}

/// debug in progress.
int main(int argc, char *argv[])
{
	int opt, port;
	bool p_opt_f = false, p_opt_p = false;
	char *pcf = NULL;

	while((opt = getopt(argc, argv, "f:p:h?")) != -1)
	{
		switch (opt)
		{
			case 'f':
				pcf = (char *) malloc(strlen(optarg) + 1);
				strcpy(pcf, optarg);
				p_opt_f = true;
				break;
			case 'p':
				p_opt_p = true;
				port = atoi(optarg);
				break;
			case 'h':
			case '?':
			default:	/* '?' */
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if(!p_opt_f)
	{
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if(!p_opt_p)
	{
		port = 60000;
		fprintf(stderr, "No port specified, Using default port : %d\n", port);
	}

	/* Init du simulateur */
	ZeArch = new arch();
	if(!(ZeArch->readCodeFile(pcf)))
	{
		fprintf(stderr, "Error during reading of code file\n");
		exit(1);
	}
	free(pcf);
	ZeArch->setExecutionMode(arch::ISS);
	ZeArch->setProgramCounter(0x0);

	/* Init du serveur TCP/IP */

	int serversock;
	struct sockaddr_in echoserver, echoclient;

	continu = true;

	signal(SIGUSR1, sig_usr1);
	signal(SIGTERM, sig_term);
	signal(SIGINT, sig_term);
	signal(SIGPIPE, sig_pipe);

/* Create the TCP socket */
	if((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		Die("Failed to create socket");
	}

/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));	/* Clear struct */
	echoserver.sin_family = AF_INET;	/* Internet/IP */
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY);	/* Incoming addr */
	echoserver.sin_port = htons(port);	/* server port */

	/* Bind the server socket */
	if(bind(serversock, (struct sockaddr *) &echoserver,
		sizeof(echoserver)) < 0)
	{
		Die("Failed to bind the server socket");
	}
	/* Listen on the server socket */
	if(listen(serversock, 1) < 0)
	{
		Die("Failed to listen on server socket");
	}

	unsigned int clientlen = sizeof(echoclient);

	/* Wait for client connection */
	printf("Waiting for GDB to connect...");
	if((GDBsock =
		accept(serversock, (struct sockaddr *) &echoclient, &clientlen)) < 0)
	{
		Die("Failed to accept client connection");
	}
	fprintf(stdout, "GDB connected: %s\n", inet_ntoa(echoclient.sin_addr));
#ifdef DEBUG
	printf("Recu Envoye\n");
#endif
	int sigval = 0;				/* Récupérer ICI la raison d'arret... Surement à passer en paramètre à handle_exception
								 * Voir dans les sources de gdb : gdb/signals/signals.c et
								 * include/gdb/signals.h */
	int h_e_ret = 0;

	/* Donne la main à GDB */
	if((h_e_ret = handle_exception(*ZeArch, sigval))<0)
		continu = 0;

	while(continu)
	{
#ifdef DEBUG
		printf("Execution du programme\n");
#endif
		while(!(sigval = ZeArch->execInst(1000))); /* Execute en boucle 1000 instruction */

#ifdef DEBUG
		printf("Programme interrompu [%d]\n", sigval);
#endif
		if(!h_e_ret && continu)
		{
			/* Donne la main à GDB */
			if((h_e_ret = handle_exception(*ZeArch, sigval)) < 0)
				continu = 0;
		}

		if(h_e_ret == 2)
		{
			fprintf(stderr, "Connection Error with GDB. Exiting the simulator\n");
			h_e_ret = 1;
			continu = 0;
		}
	}

	if(!h_e_ret)putpacket("X12");


	close(GDBsock);
	close(serversock);

	return EXIT_SUCCESS;
}

int getDebugChar(unsigned char * c)
{
	struct timeval tv;
	fd_set fds;
	int select_ret=1;
	*c = 0;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;

	FD_ZERO(&fds);
	FD_SET(GDBsock,&fds);

	/*Attente de pourvoir lire sur GDBsock ou l'arret de la fonction */
	while((select_ret= select(GDBsock + 1, &fds, NULL, NULL, &tv)) == 0)
	{
		if(!continu || sig_pipe_received || select_ret < 0)
			break;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		FD_ZERO(&fds);
		FD_SET(GDBsock,&fds);
	}

	/* Get one char */
	if(!sig_pipe_received && continu && select_ret > 0)
	{
		unsigned int ret;
		if((ret = recv(GDBsock, c, 1, 0)) < 1)
		{
			if(ret != 0)
				perror("Failed to receive from GDB");
			return 1;
		}
	}
	else return 1;
#ifdef DEBUG
	if(*c == '$')
		printf("\n");
	printf("%c", *c);	/* Affiche c en rouge */
	static unsigned char count;
	if(*c == '#')
		count = 1;
	else if(count)
		count++;
	if(count == 3)
	{
		fflush(stdout);
		count = 0;
	}
	/* fflush sur acquittement */
	if(*c == '+' || *c == '-')
		fflush(stdout);
#endif
	return 0;
}

int putDebugChar(int c)
{

	struct timeval tv;
	fd_set fds;
	int select_ret=1;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;

	FD_ZERO(&fds);
	FD_SET(GDBsock,&fds);

	/*Attente de pourvoir lire sur GDBsock ou l'arret de la fonction */
	while((select_ret= select(GDBsock + 1, NULL, &fds, NULL, &tv)) == 0)
	{
		if(!continu || sig_pipe_received || select_ret < 0)
			break;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		FD_ZERO(&fds);
		FD_SET(GDBsock,&fds);
	}

	if(!sig_pipe_received && continu && select_ret > 0)
	{
		if(send(GDBsock, &c, 1, 0) != 1)
		{
			perror("Failed to send bytes to client");
			return 1;
		}
	}else return 1;
#ifdef DEBUG
	static unsigned char count;
	if(c == '$')
		printf("\n");
	printf("%c", c);	/* Affiche c en bleu */
	/* fflush stdout sur fin de trame */
	if(c == '#')
		count = 1;
	else if(count)
		count++;
	if(count == 3)
	{
		fflush(stdout);
		count = 0;
	}

	/* fflush sur acquittement */
	if(c == '+' || c == '-')
		fflush(stdout);
#endif
	return 0;
}

