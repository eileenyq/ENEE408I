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

// Struct of the data packet to be sent between the PC and the ESP32
struct __attribute__((packed)) Data {
    int16_t seq;     // sequence number to track packet order
    int32_t distance; // distance
    float voltage;   // voltage
    char text[50];   // any text
};

int main () {
    int server_sd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_sd == -1) {
        printf("socket creation failed"); 
    }
    struct sockaddr_in server_addr, client_addr;
    //127.0.1.1
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(2020); 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("128.8.126.63"); //10.104.134.145

    int n = bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)); 
    if (n < 0) {
        perror("bind()");
        exit(0);
    }
    listen(server_sd, 5);//we will have at most 2 clients, can change later
    printf("Start listening for messages forever\n");
    char * buffer = malloc(1024);
    char ack[4] = "ACK";
    socklen_t client_len = sizeof(client_addr);
    int client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
    while(1) {
        memset(buffer, 0, 1024);
        int retval = recv (client_sd, buffer, 1024, 0);
        //printf("read %d bytes\n", retval)
        printf("buffer is %s\n", buffer);
        //int s = sendto(client_sd, ack, 10, 0, (struct sockaddr *)&client_addr, client_len);
        //printf("sent %d bytes\n", s);
    }
    return 0;
}