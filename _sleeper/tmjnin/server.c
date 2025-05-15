#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/*socket → bind → listen → accept */
int main()
{
    char buffer[10000];

    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1234);
    server_addr.sin_addr.s_addr =INADDR_ANY;
    memset (server_addr.sin_zero, 0 , sizeof(server_addr.sin_zero));// useless

    // bind() dit : « Ce socket écoutera sur IP:PORT indiqués par server_addr ».
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //kat assigni port ou IP
    listen(server_fd, 3);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    printf("Server Listen !!\n");

    //bloque server ou ka ycreer cnx fidel m3a client
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);  
    printf("Client connected !!\n");

    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("buffer : %s\n", buffer);
    return 0;

}