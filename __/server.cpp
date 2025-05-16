#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "client.hpp"
#include <map>

std::map<int, client> clients;

void accept_new_connection(int server_socket, fd_set *all_sockets, int *fd_max)
{
    int client_fd;
    char msg_to_send[BUFSIZ];
    int status;

    client_fd = accept(server_socket, NULL, NULL);
    clients[client_fd] = client();
    if (client_fd == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }
    FD_SET(client_fd, all_sockets); // Ajoute la socket client à l'ensemble
    if (client_fd > *fd_max) {
        *fd_max = client_fd; // Met à jour la plus grande socket
    }
    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);
}

int read_data_from_socket(int socket, fd_set *all_sockets, int fd_max, int server_socket)
{
    char buffer[12];
    // char msg_to_send[1024];
    int bytes_read;
    int status;
    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(socket, buffer, 10, 0);
	// std::cout << strlen(buffer) << std::endl;
	// if(strlen(buffer) != 0)
	clients[socket].setBuffer(buffer);
	// std::string c = buffer;
	// if(c.find("\r\n") == std::string::npos)
	// 	return 0;
    // std::cout << "buffer --> " << buffer << "-----\n";
    // if (bytes_read <= 0) {
    //     if (bytes_read == 0) {
            // printf("[%d] Client socket closed connection.\n", socket);
    //     }
    //     else {
    //         fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));
    //     }
    //     close(socket); // Ferme la socket
    //     FD_CLR(socket, all_sockets); // Enlève la socket de l'ensemble
    // }
    // else 
	// {
    //     printf("[%d] Got message: %s", socket, buffer);
	// 	std::string message = "Hello, server!";
	// 	ssize_t bytesSent = send(socket, message.c_str(), message.length(), 0);
	// 	if (bytesSent < 0) {
	// 		std::cerr << "Send failed.\n";
	// 	} else {
	// 		std::cout << "Sent " << bytesSent << " bytes.\n";
	// 	}
	// }
	return 1;
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
	int flag = 0;
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
		// std::cout << "hh\n";
		for (int i = 0; i <= fd_max; i++)
		{
			if(!FD_ISSET(i, &read_fds))
				continue;
			if(i == socket_fd)
				accept_new_connection(socket_fd, &all_sockets, &fd_max);
			else
                flag = read_data_from_socket(i, &all_sockets, fd_max, socket_fd);
		}
	}
	return 0;
}
