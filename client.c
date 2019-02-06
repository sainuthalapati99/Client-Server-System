#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 3456 
#define MSG_SIZE 1024
int main(int argc, char const *argv[])
{
	struct sockaddr_in address;
	int sockfd = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	int fd;
	fd_set readfds, testfds, clientfds;
	char msg[MSG_SIZE + 1];
	char kb_msg[MSG_SIZE + 10];
	int result;

	char buffer[1024] = { 0 };
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	fflush(stdout);

	FD_ZERO(&clientfds);
	
	FD_SET(sockfd, &clientfds);
	
	FD_SET(0, &clientfds);//stdin
	

	
	while (1) {
		
		testfds = clientfds;
		select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
		
		for (fd = 0; fd<FD_SETSIZE; fd++) {
			//printf("in   for loop%d",fd);
			if (FD_ISSET(fd, &testfds)) {
				if (fd == sockfd) {   /*Accept data from open socket */
									  //printf("client - read\n");

									  //read data from open socket
					result = read(sockfd, msg, MSG_SIZE);
					msg[result] = '\0';  /* Terminate string with null */
					printf("%s", msg );

					if (msg[0] == 'X') {
						close(sockfd);
						exit(0);
					}
				}
				else if (fd == 0) { /*process keyboard activiy*/
									// printf("client - send\n");

					//printf("enter a message:");
					fgets(kb_msg, MSG_SIZE , stdin);
					//printf("%s\n",kb_msg);
					if (strcmp(kb_msg, "quit\n") == 0) {
						sprintf(msg, "client is shutting down.\n" );
						write(sockfd, msg, strlen(msg));
						close(sockfd); //close the current client
						exit(0); //end program
					}
					else {
						/* sprintf(kb_msg,"%s",alias);
						msg[result]='\0';
						strcat(kb_msg,msg+1);*/

						sprintf(msg, "%s", kb_msg);
						write(sockfd, msg, strlen(msg));
					}
				}
			}
		}
	}

	return 0;
}
