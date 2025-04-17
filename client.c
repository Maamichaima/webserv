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
	socklen_t addr_size;
    struct sockaddr_storage client_addr;
	int status;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1234);
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if(socket_fd == -1)
	{
		printf("socket error\n");
		return 1;
	}

	status = connect(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
	if(status == -1)
	{
		printf("connect error\n");
		return 1;
	}

	//send()
	char *msg = "Hello, server!";
	int msg_len = strlen(msg);
	status = send(socket_fd, msg, msg_len, 0);
	if(status == -1)
	{
		printf("send error\n");
		return 1;
	}
}