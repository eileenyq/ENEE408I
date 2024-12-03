
#include "client.hpp"

extern int lineStatus;
// WiFi network credentials
const char* ssid = "Eileenphone";
const char* password = "123456789";


// Server IP and port
const char* host = "172.20.10.8";  // Replace with the IP address of server
const uint16_t port = 2024;

// Create a client
WiFiClient client;

void client_setup() {
  Serial.begin(115200);
   delay(1000);

  WiFi.begin(ssid, password);//add
  Serial.print("Connecting to ");
  Serial.println(ssid);
   int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(1000);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Please check your credentials or signal strength.");
  }//add

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
    Serial.println("test");
  }

  if (WiFi.status() != WL_CONNECTED) { //add
    Serial.println("WiFi disconnected, attempting to reconnect..."); // add
    WiFi.reconnect(); //add
  }

  Serial.println("Connected to WiFi!");

  // Connect to the server
  if (client.connect(host, port)) {
    Serial.println("Connected to server!");
    //send test message
    char msg[50];
    strcpy(msg, "Message From Client\n");
    sendMsg(msg, 2);
  } else {
    Serial.println("Connection to server failed.");
    return;
  }
}
void sendMsg(char * msg, char status) {
  while (!client.connected()) {
      //Serial.println("Disconnected from server. Attempting to reconnect...");
      // client.stop();
      if (client.connect(host, port)) {
          Serial.println("Reconnected to server.");
      } else {
          Serial.println("Reconnect failed.");
          //delay(5000); // Retry delay
      }
  }
  if (client.connected()) {
    //send data
    struct Data d;

    d.status = status;
    memset(d.text, 0, 50);
    strcpy(d.text, msg);
    client.write((char*)&d, sizeof(d));
    Serial.printf("Sent %s\n", d.text);
  } else {
    Serial.printf("%s\n", msg);
  }
}

char client_checkformsgs() {
    // Ensure connection is established with the server
    if (!client.connected()) {
        //Serial.println("Disconnected from server. Attempting to reconnect...");
        // client.stop();
        if (client.connect(host, port)) {
            Serial.println("Reconnected to server.");
        } else {
            Serial.println("Reconnect failed.");
            //delay(5000); // Retry delay
            return('x');
        }
    }
    // Check if connected to the server
    if (client.connected()) {
        // Read server's response
        if (client.available() > 0) {
            char buffer[1];
            client.readBytes(buffer, 1); // Read data from server and unpack it
            Serial.printf("Received response - imageDetected %c\n", buffer[0]);
            return(buffer[0]);
        }
        //delay(5000); // Delay to avoid flooding server
    }
    return('x');
}
