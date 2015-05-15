/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application serveur*
 *              application client                                   *
 *                                                                   *
 * principaux appels (du point de vue serveur) pour un protocole     *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     bind()                                                        *
 *                                                                   * 
 *     listen()                                                      *
 *                                                                   *
 *     accept()                                                      *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *********************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define PF_INET6 AF_INET6
#define MAXLINE 80
#define MAXSOCK 5

void usage(){
  printf("usage : servecho numero_port_serveur\n");
}


int main (int argc, char *argv[])

{
	int serverSocket, clientSocket, nsock; /* declaration socket passive et socket active */
	int n, retread, childpid, error;
	struct addrinfo hints, *res, *res0;
	socklen_t clilen;
	struct sockaddr_in6 cli_addr;
	char fromClient[MAXLINE];
	char fromUser[MAXLINE];

	/* Verifier le nombre de paramètre en entrée */
	/* serverTCP <numero_port>                   */ 

		if (argc != 2){
		usage();
		//exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(NULL, argv[1], &hints, &res0);
	if (error) {
		errx(1, "%s", gai_strerror(error));
	}
	nsock = 0;
	for (res = res0; res && nsock < MAXSOCK; res = res->ai_next) {
		if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
			perror("servecho : Probleme socket\n");
			exit (2);
		}

		if (bind(serverSocket,res->ai_addr, res->ai_addrlen ) <0) {
			perror ("servecho: erreur bind\n");
			exit (1);
		}

		if (listen(serverSocket, SOMAXCONN) <0) {
			perror ("servecho: erreur listen\n");
			exit (1);
		}

		nsock ++;
	}
	if (nsock == 0) {
		exit(1);
	}

	clilen = sizeof(cli_addr);
	clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
	if (clientSocket < 0) {
		perror("servecho : erreur accept\n");
		exit (1);
	}


	/* Envoyer Bonjour au client */
	if ( (n= writen (clientSocket, "Bonjour\n", strlen("Bonjour\n")) ) != strlen("Bonjour\n"))  {
		printf ("erreur writen");
		exit (1);
	 }

  while ( (retread=readline (clientSocket, fromClient, MAXLINE)) > 0)
	{
		printf ("corr: %s", fromClient);
		if (strcmp (fromClient,"Au revoir\n") == 0) 
			break ; /* fin de la lecture */

		/* saisir message utilisateur */
		printf("vous: ");
		if (fgets(fromUser, MAXLINE,stdin) == NULL) {
			perror ("erreur fgets \n");
			exit(1);
		}

		/* Envoyer le message au client */
      if ( (n= writen (clientSocket, fromUser, strlen(fromUser), 0) ) != strlen(fromUser))  {
			printf ("erreur writen");
			exit (1);
		}

	}

	if(retread < 0 ) {
		perror ("erreur readline \n");
		 exit(1);
	}
	close(serverSocket);
	close(serverSocket);


}
