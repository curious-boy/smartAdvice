/* a sample server in the internet domain using TCP
	The port number is passed as an argument */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>		//为正常输出ip地址添加
#include <netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void dostuff(int);	// function prototype

int main(int argc, char *argv[])
{
	int sockfd,newsockfd,portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if(argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0)
	{	
		error("ERROR opening socket");
	}

	bzero((char*)&serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd,(struct sockaddr *)&serv_addr,
		sizeof(serv_addr)) < 0)
	{
		error("ERROR on binding");
	}
	/* set the max connector */
	if (listen(sockfd,5) == -1)
	{
		error("ERROR on listen");
	}
	//listen(sockfd, 5);

	// block server until client app create conn
	clilen = sizeof(struct sockaddr_in);

	// use multi process to handle multi connection
#define MULTIPROCESS 0

#if MULTIPROCESS
	int pid;
	while(1)
	{
		newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
		if (newsockfd < 0 )
		{
			error("ERROR on accept");
		}

		fprintf(stderr,"Server get connection from %s\n",inet_ntoa(cli_addr.sin_addr)); 

		pid = fork();
		if (pid < 0 )
		{
			error("ERROR on fork");
		}
		if (pid == 0)
		{
			close(sockfd);
			dostuff(newsockfd);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}

#else
	// single process to handle connections
	while(1)
	{
		// if ((newsockfd=accept(sockfd,(struct sockaddr *)(&cli_addr),&clilen))<0)
		if((newsockfd=accept(sockfd,(struct sockaddr *)(&cli_addr),&clilen))==-1) 
		{
			error("ERROR on accept");
			// exit(1);
		}

		fprintf(stderr,"Server get connection from %s\n",inet_ntoa(cli_addr.sin_addr)); 

		// trans net address to str
		if ((n = read(newsockfd,buffer,1024))==-1)
		{
			error("ERROR on read");
			// exit(1);
		}
		buffer[n]='\0';
		printf("Here is the message: %s\n", buffer);

		n = write(newsockfd,"I got your message", 18);
		if (n<0)
		{
			error("ERROR writing to socket");
		}

		close(newsockfd);
	}
#endif
	close(sockfd);

	return 0;

}

// for multi process to handle multi net connections
void dostuff(int sock)
{
	int n;
	char buffer[256];

	bzero(buffer,256);
	n = read(sock,buffer,255);
	if (n<0)
	{
		error("ERROR reading from socket");
	}
	printf("Here is the message: %s\n", buffer);
	n = write(sock,"I got your message",18);
	if (n < 0)
	{
		error("ERROR writing to socket");
	}
}