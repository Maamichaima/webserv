#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int c, char **v)
{
	struct sockaddr_in sa;
	int socket_fd;
	int client_fd;   
	socklen_t addr_size;
    struct sockaddr_storage client_addr;
	int status;

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

	
	//accepet()
	printf("En attente de connexion...\n");
	addr_size = sizeof client_addr;
	client_fd = accept(socket_fd, (struct sockaddr *)&sa, &addr_size);
	if(client_fd == -1)
	{
		printf("accept error\n");
		return 1;
	}
	printf("sffff rat taccepta\n");

	//recv()
	char buffer[1024];
	int byt_recu = recv(client_fd, buffer, sizeof(sa), 0);
	if(byt_recu == -1)
	{
		printf("recv error\n");
		return 1;
	}
	printf("bytes reçus : %d\n", byt_recu);
	printf("message : %s\n", buffer);
	close(client_fd);
	close(socket_fd);
	return 10;
}