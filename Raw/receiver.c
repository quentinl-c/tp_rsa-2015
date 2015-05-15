#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>

#define GROUP "239.137.194.111"
#define PORT 88888 
#define BUFF_SIZE 2048


int main(int argc, char *argv[]){

	int sock_r, dialogSocket, clilen, newsockfd, n, fromlen;
	char buffer[BUFF_SIZE], *ip_string;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	struct hostent *hostname;
	struct in_addr **addr_list;
	struct ip_mreq imr;

	if((sock_r = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		perror ("erreur socket\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	imr.imr_multiaddr.s_addr = inet_addr(GROUP);
	imr.imr_interface.s_addr = INADDR_ANY;

	unsigned int on = 1;
	if (setsockopt(sock_r, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
		perror("probléme setsockopt");
		exit(1);
	}


	if(setsockopt(sock_r, IPPROTO_IP,IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) < 0){
		perror("probléme setsockopt");
		exit(1);
	}

	if (bind(sock_r, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("serverecho : erreur bind \n");
		exit(1);
	}


	clilen = sizeof(cli_addr);


	memset(buffer, 0, sizeof(buffer));
	int len = sizeof(serv_addr);

	if(recvfrom(sock_r, buffer, BUFF_SIZE, 0, (struct sockaddr *) &serv_addr, &len) < 0){
		perror("cliecho : erreur recv");
		exit(1);
	}

	printf("%s\n", buffer);

	return 0;
}
