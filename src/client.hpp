#include <WiFi.h>

// Define the structure of the data packet
struct Data {
    int16_t status;
    char text[50];   // text
};

void client_setup();
void client_checkformsgs();
void sendMsg(char * msg);
