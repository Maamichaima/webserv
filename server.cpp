#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

void accept_new_connection(int server_socket, fd_set *all_sockets, int *fd_max)

{
    int client_fd;
    char msg_to_send[BUFSIZ];
    int status;

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }
    FD_SET(client_fd, all_sockets); // Ajoute la socket client à l'ensemble
    if (client_fd > *fd_max) {
        *fd_max = client_fd; // Met à jour la plus grande socket
    }
    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);
    // memset(&msg_to_send, '\0', sizeof msg_to_send);
    // sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", client_fd);
    // status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
    // if (status == -1) {
    //     fprintf(stderr, "[Server] Send error to client %d: %s\n", client_fd, strerror(errno));
    // }
}

int main(int c, char **v)
{
	struct sockaddr_in sa;
	int socket_fd;
	int client_fd;   
	socklen_t addr_size;
    struct sockaddr_storage client_addr;
	int status;

	fd_set all_sockets;
	fd_set read_fds;
    struct timeval timer;
	int fd_max;


	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1234);
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	//socket()
	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if(socket_fd == -1)
	{
		printf("socket error\n");
		return 1;
	}


	//bind()
	// status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
	status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
	if(status == -1)
	{
		printf("bind error\n");
		return 1;
	}

	//listen()
	status = listen(socket_fd, 10);
	if(status == -1)
	{
		printf("listen error\n");
		return 1;
	}

	FD_ZERO(&read_fds);
	FD_ZERO(&all_sockets);
	FD_SET(socket_fd, &all_sockets);
	fd_max = socket_fd;
	while(1)
	{
		read_fds = all_sockets;
		timer.tv_sec = 2;

        timer.tv_usec = 0;
		status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
		if (status == -1) 
		{
            std::cout << "[Server] Select error";
            exit(1);
        }
        else if (status == 0) 
		{
            // Aucun descipteur de fichier de socket n'est prêt pour la lecture
            std::cout << "[Server] Waiting...\n";
            continue;
        }
		for (int i = 0; i <= fd_max; i++)
		{
			if(FD_ISSET(i, &read_fds) != 1)
				continue;
			if(i == socket_fd)
			{
				accept_new_connection(socket_fd, &all_sockets, &fd_max);
			}
			else
			{

			}
		}
	}

	//accepet()
	// printf("En attente de connexion...\n");
	// addr_size = sizeof client_addr;
	// client_fd = accept(socket_fd, (struct sockaddr *)&sa, &addr_size);
	// if(client_fd == -1)
	// {
	// 	printf("accept error\n");
	// 	return 1;
	// }
	// printf("sffff rat taccepta\n");

	// //recv()
	// char buffer[1024];
	// int byt_recu = recv(client_fd, buffer, sizeof(sa), 0);
	// if(byt_recu == -1)
	// {
	// 	printf("recv error\n");
	// 	return 1;
	// }
	// printf("bytes reçus : %d\n", byt_recu);
	// printf("message : %s\n", buffer);
	// close(client_fd);
	// close(socket_fd);
	return 10;
}