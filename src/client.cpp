
#include "client.hpp"


// WiFi network credentials
const char* ssid = "";
const char* password = "";


// Server IP and port
const char* host = "192.168.0.74";  // Replace with the IP address of server
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
    client.write("Message From Client\n", 20);
  } else {
    Serial.println("Connection to server failed.");
    return;
  }
}
void sendMsg(Data * data) {
  if (client.connected()) {
    //send data
    client.write((char*)data, sizeof(data));
    Serial.printf("Sent %s", data->text);
  } else {
    Serial.printf("%s\n", data->text);
  }
}

void client_checkformsgs() {
    // Ensure connection is established with the server
    if (!client.connected()) {
        Serial.println("Disconnected from server. Attempting to reconnect...");
        client.stop();
        delay(1000); // Short delay before reconnecting
        if (client.connect(host, port)) {
            Serial.println("Reconnected to server.");
        } else {
            Serial.println("Reconnect failed.");
            delay(5000); // Retry delay
            return;
        }
    }
    // Check if connected to the server
    if (client.connected()) {
        // Read server's response 
        if (client.available() > 0) {
            Data response;
            client.readBytes((char*)&response, sizeof(response)); // Read data from server and unpack it
            Serial.printf("Received response - imageDetected %d, text %s\n",
                          (int)response.status, response.text);
        }
        //delay(5000); // Delay to avoid flooding server
    }
}
