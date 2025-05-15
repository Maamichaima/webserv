#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main () {
    char buffer[1000] = "amine";
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1234);
    
    //convertir !!
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    send(sockfd, buffer, strlen(buffer), 0);
}