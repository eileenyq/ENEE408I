#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <argp.h>
#include <netdb.h>
#include <errno.h>
#include <assert.h>
#include <openssl/evp.h>

#define PORT 2020
int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("10.104.134.145");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    while(1) {
        char buff[10] = "hi";
        char rec[4];
        sendto(sockfd, buff, 10, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        recv(sockfd, rec, 4, 0);
        printf("received %s\n", rec);
        sleep(1);
    }
    // close the socket
    close(sockfd);
}