#include <unistd.h>  // local connection
#include <stdio.h> 
#include <sys/socket.h>  // socket library
#include <stdlib.h> 
#include <netinet/in.h>  // internet structure
#include <string.h> 
#define PORT 3456 
#define MAX_CLIENTS 5
#define MSG_SIZE 1024

int main(int argc, char const *argv[])
{
	int server_fd, client_sockfd;
	struct sockaddr_in address; //local adress
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char server_message[1024] = "Hello from server";

	int fd;
	char fd_array[MAX_CLIENTS];
	char client_names[MAX_CLIENTS][1024];
	fd_set readfds, testfds;
	int num_clients = 0;


	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}


	address.sin_family = AF_INET;           //assigning local netwrok
	address.sin_addr.s_addr = INADDR_ANY;      // assigning local adress
	address.sin_port = htons(PORT);        //assign port 3456

	// Forcefully attaching socket to the port 3456 / binding
    //Bind the socket to an address using the bind() system call. For a server socket on the Internet, an address consists of a port number on the host machine.
	if (bind(server_fd, (struct sockaddr *)&address,
		sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

    FD_ZERO(&readfds);      // FD_ZERO(&fdset) Initializes the file descriptor set fdset to have zero bits for all file descriptors.
	FD_SET(server_fd, &readfds);
	FD_SET(0, &readfds);//stdin

	while (1)
	{
		char msg[1024], kb_msg[1024];
		testfds = readfds;
		select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {

				if (fd == server_fd) { /* Accept a new connection request */
					client_sockfd = accept(server_fd, NULL, NULL);

					if (num_clients < MAX_CLIENTS) {

						FD_SET(client_sockfd, &readfds);
						fd_array[num_clients] = client_sockfd;

						
						/*Client ID*/
						printf("Client %d joined\n", num_clients);
						fflush(stdout);
						//printf("Client %d joined\n", num_clients);
						//sprintf(msg, "%s", "What is your name?");
						//send(client_sockfd, msg, strlen(msg), 0);
						num_clients++;
						//printf("Client %d joined\n", num_clients);
					}
					else {
						sprintf(msg, "sorry, too many clients.  try again later.\n");
						write(client_sockfd, msg, strlen(msg));
						close(client_sockfd);
					}
				}
				else if (fd == 0) {  /* Process keyboard activity */
					fgets(kb_msg, MSG_SIZE + 1, stdin);

					if (strcmp(kb_msg, "quit\n") == 0) {
                       
						sprintf(msg, "server is shutting down.\n");
                        //If client neters quit servers sned a message to all the clinets
						for (int i = 0; i < num_clients; i++) {
							write(fd_array[i], msg, strlen(msg));
							close(fd_array[i]);
						}
						close(server_fd);
						exit(0);
					}
					else {
                        sprintf(msg, "Server says:%s", kb_msg);
						for (int i = 0; i < num_clients; i++)
							write(fd_array[i], msg, strlen(msg));
					}
				}
				else if (fd) {  /*Process Client specific activity*/
								//printf("server - read\n");
								//read data from open socket
					int result = read(fd, msg, MSG_SIZE);

					if (result == -1) perror("read()");
					else if (result > 0)
					{
						char newmsg[1024];
						int clientid = 0;

						msg[result] = '\0';
						//printf(msg);
						for (int i = 0; i < num_clients; i++)
						{
							if (fd_array[i] == fd) 
							{
								clientid = i;
							}

						}
						sprintf(newmsg, "client %d says: %s", clientid, msg);
						for (int i = 0; i < num_clients; i++)
						{
							 if (fd_array[i] != fd)  /*dont write msg to same client*/
								 {
									 sprintf(newmsg, "client %d says: %s", clientid, msg);
									 write(fd_array[i], newmsg, strlen(newmsg));
								 }


						}


					}
                    else{
                        int clientid = 0;
                        for (int i = 0; i < num_clients; i++)
                        {
                            if (fd_array[i] == fd)
                            {
                                clientid = i;
                            }
                        }
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("client %d has disconnected\n", clientid);
                         num_clients--;
                        fflush(stdout);
                         //num_clients--;
                    }

				}
			}
		}
	}



	close(server_fd);
    return 0; 
} 
