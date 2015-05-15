#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>

#define SERV_PORT 2222
#define BUFF_SIZE 2048


int main(int argc, char *argv[]){

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	struct hostent *hostname;
	struct in_addr **addr_list;

	int serverSocket, dialogSocket, clilen, newsockfd, n, fromlen;

	char buffer[BUFF_SIZE], *ip_string;


	if((serverSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		perror ("erreur socket\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);


	if (bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("serverecho : erreur bind \n");
		exit(1);
	}


	clilen = sizeof(cli_addr);


	memset(buffer, 0, sizeof(buffer));
	fromlen = sizeof(cli_addr);

	if(recvfrom(serverSocket, buffer, BUFF_SIZE, 0, (struct sockaddr *) &cli_addr, &fromlen) < 0){
		perror("cliecho : erreur recv");
		exit(1);
	}

	if ( (n= sendto (serverSocket, buffer, strlen(buffer),0, 
		(struct sockaddr *)&cli_addr, sizeof(serv_addr)
		)) != strlen(buffer))  {
		perror ("erreur sendto");
	exit; 
}

printf("%s\n", buffer);



return 0;
}
