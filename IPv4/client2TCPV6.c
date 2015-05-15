/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client) pour un protocole      *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     connect()                                                     *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <arpa/inet.h>
#include     <strings.h>
#include      <netdb.h>
#include      <stdlib.h>

#define MAXLINE 80

void usage(){
  printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}
int main (int argc, char *argv[]){

	int serverSocket, servlen, n, retread, error;
	struct addrinfo hints, *res, *res0;
	char fromServer[MAXLINE];
	char fromUser[MAXLINE];
	struct hostent *hp;  

	/* Verifier le nombre de paramètre en entrée */
	/* clientTCP <hostname> <numero_port>        */
	if (argc != 3){
		usage();
		exit(1);
	}

	/* 
	 * Remplir la structure  serv_addr avec l'adresse du serveur 
	 */
	memset ( &hints, 0, sizeof(hints) );
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(argv[1], argv[2], &hints, &res0);
	 
	 if (error) {
	 	perror("error getaddr");
		exit(1);
	}
	/*
	 * Ouvrir socket (socket STREAM)
	 */
	for (res = res0; res; res = res->ai_next) {
		if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
			perror ("erreur socket");
			exit (1);
		}
		if (connect (serverSocket, res->ai_addr, res->ai_addrlen) < 0){
			perror ("erreur connect");
			exit (1);
		}
		break;
	}

	/*
	 * Connect to the serveur 
	 */
	

	while ( (retread =readline (serverSocket, fromServer, MAXLINE)) > 0){
		printf ("corr: %s", fromServer);
		if (strcmp (fromServer,"Au revoir\n") == 0) 
			break ; /* fin de la lecture */

		/* saisir message utilisateur */
		printf("vous: ");
		if (fgets(fromUser, MAXLINE,stdin) == NULL) {
			perror ("erreur fgets \n");
			exit(1);
		}
		
		/* Envoyer le message au serveur */
		if ( (n= writen (serverSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
			printf ("erreur writen");
			exit (1);
		}
	 }
	if(retread < 0 ) {
		perror ("erreur readline \n");
	//	exit(1);
	}

	close(serverSocket);
}
