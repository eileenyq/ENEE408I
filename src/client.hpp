#include <WiFi.h>

// Define the structure of the data packet
struct Data {
    int16_t status;
    char text[50];   // text
};

void client_setup();
char client_checkformsgs();
char client_checkformsgs_blocking();
void sendMsg(char * msg, char status);
