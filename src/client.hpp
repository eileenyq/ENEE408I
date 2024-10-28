#include <WiFi.h>

// Define the structure of the data packet
struct __attribute__((packed)) Data {
    int16_t status;
    // int32_t distance; // distance
    // float voltage;   // voltage
    char text[50];   // text
};

void client_setup();
void client_checkformsgs();
void sendMsg(Data * data);
