
#include "client.hpp"

// Define the structure of the data packet
struct __attribute__((packed)) Data {
    int16_t seq;     // sequence number
    int32_t distance; // distance
    float voltage;   // voltage
    char text[50];   // text
};

// WiFi network credentials
const char* ssid = "MOTO7030";
const char* password = "hvpzmaks3a";


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
  } else {
    Serial.println("Connection to server failed.");
    return;
  }
}

void client_loop() {
    // Prepare data packet
    Data data;
    data.seq = 1;
    data.distance = 1000;
    data.voltage = 3.7f;
    strncpy(data.text, "Hello from ESP32!", sizeof(data.text) - 1);
    data.text[sizeof(data.text) - 1] = '\0';

    //Serial.printf("seq %d distance %ld voltage %f text %s\n", data.seq, data.distance, data.voltage, data.text);

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
        // Send data to the server
        client.write((char*)&data, sizeof(data));


        // Read server's response 
        if (client.available() > 0) {
            Data response;
            client.readBytes((char*)&response, sizeof(response)); // Read data from server and unpack it
            Serial.printf("Received response - seq %d, distance %ld, voltage %f, text %s\n",
                          (int)response.seq, (long)response.distance, response.voltage, response.text);
        }

        // Increment sequence number for next packet
        data.seq++;
        delay(5000); // Delay to avoid flooding server
    }
}
