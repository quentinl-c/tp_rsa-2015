/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_URGENT_TCP                                        *
 *                                                                   *
 * DESCRIPTION: Utilisation de la donnee urgente de TCP              *
 *                                                                   *
 *********************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

int serverSocket, clientSocket; /* declaration socket passive et socket active */


 void sig_urg(int sig)
 {
	int n=1;
	char buff[2048];
	printf ("SIGURG recu \n");

	if ( (n= read ( clientSocket, buff, sizeof(buff)-1) ) < 0 )  {
			perror ("SERVER_URGENT_TCP :  readn error on socket");
			exit (1);
		}
		buff[n]='\0';

	printf("Recepteur: recu signal SIGURG (%d): caractere urgent\n",sig);
	printf ("read %d OOB octets %s\n", n,buff);
}

usage(){
	printf("usage : servecho numero_port_serveur\n");
}


int main (int argc, char *argv[]){
	struct sigaction action;

	int n, clilen, servlen, size, nrcv;
	struct sockaddr_in  serv_addr, cli_addr;
	char buff[2048];
	socklen_t optionlen;

	/* Verifier le nombre de paramètre en entrée */
	/* serverTCP <numero_port>                   */ 
	if (argc != 2){
		usage();
		exit(1);
	}

  // Ouvrir une socket (a TCP socket)
	if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) <0) {
		perror("servecho : Probleme socket\n");
		exit (2);
	}

	// Lier l'adresse  locale à la socket
	memset( (char*) &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

 if (bind(serverSocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
	perror ("servecho: erreur bind\n");
	exit (1);
 }
 
  // Obtenir taille buffer de reception
	if(getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &size, &optionlen) < 0){
		perror("servecho : erreur getsockopt");
		exit(1);
	}

	printf (" la taille du buffer de reception est de %d \n", size);


 
	// Paramètrer le nombre de connexion "pending" 
	if (listen(serverSocket, SOMAXCONN) <0) {
		perror ("servecho: erreur listen\n");
		exit (1);
	}

	// Attente de la demande de connexion du client
	clilen = sizeof(cli_addr);
	clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
	if (clientSocket < 0) {
		perror("servecho : erreur accept\n");
		exit (1);
	}

	fcntl(clientSocket, F_SETOWN, getpid());

	// Armer le signal SIGUR avec comme handler sig_urg
	action.sa_handler = sig_urg;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	sigaction(SIGURG,&action,NULL);


 
  // Demander au systeme d'envoyer le signal au proprietaire de la socket
  // A COMPLETER


	 for (;;){
		if ( (nrcv= read ( clientSocket, buff, sizeof(buff)-1) ) < 0 )  {
			perror ("SERVER_URGENT_TCP :  readn error on socket\n");
			exit (1);
		}
		buff[nrcv]='\0';
		if(errno == EINTR){
			continue;
		}
		printf ("SERVER_URGENT_TCP : message recu=%s  nrcv = %d \n",buff, nrcv);
	}
}