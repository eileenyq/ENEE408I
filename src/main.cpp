#include <Arduino.h>
#include <Adafruit_MCP3008.h>
#include <Encoder.h>
#include <WiFi.h>

#define CLIENT /*comment out this line if we are runnning without wifi*/

WiFiClient client;
const char* host = "172.20.10.3";  // Replace with the IP address of server
const uint16_t port = 2024;

const char* ssid = "Eileenphone";
const char* password = "123456789";

// ADC (line sensor)
Adafruit_MCP3008 adc1;
Adafruit_MCP3008 adc2;

const unsigned int ADC_1_CS = 2;
const unsigned int ADC_2_CS = 17;

int adc1_buf[8];
int adc2_buf[8];

int lineStatus;
uint8_t lineArray[13];

// Encoders
const unsigned int M1_ENC_A = 39;
const unsigned int M1_ENC_B = 38;
const unsigned int M2_ENC_A = 37;
const unsigned int M2_ENC_B = 36;

// Motors
const unsigned int M1_IN_1 = 13;
const unsigned int M1_IN_2 = 12;
const unsigned int M2_IN_1 = 25;
const unsigned int M2_IN_2 = 14;

const unsigned int M1_IN_1_CHANNEL = 8;
const unsigned int M1_IN_2_CHANNEL = 9;
const unsigned int M2_IN_1_CHANNEL = 10;
const unsigned int M2_IN_2_CHANNEL = 11;

const unsigned int M1_I_SENSE = 35;
const unsigned int M2_I_SENSE = 34;

const unsigned int PWM_MAX = 255;
const int freq = 5000;
const int resolution = 8; // 8-bit resolution -> PWM values go from 0-255

// LED
const int ledChannel = 0;

void M2_stop();
void M1_stop();

/*
 *  Movement functions
 */
void M1_forward(int pwm_value) {
  ledcWrite(M1_IN_1_CHANNEL, 0);
  if (pwm_value > PWM_MAX) {
    ledcWrite(M1_IN_2_CHANNEL, PWM_MAX);
  } else {
    ledcWrite(M1_IN_2_CHANNEL, pwm_value);
  }
}
void M2_forward(int pwm_value) {
  ledcWrite(M2_IN_1_CHANNEL, 0);
  if (pwm_value > PWM_MAX) {
    ledcWrite(M2_IN_2_CHANNEL, PWM_MAX);
  } else {
    ledcWrite(M2_IN_2_CHANNEL, pwm_value);
  }
}

void M1_backward(int pwm_value) {
  ledcWrite(M1_IN_1_CHANNEL, pwm_value);
  ledcWrite(M1_IN_2_CHANNEL, 0);
}
void M2_backward(int pwm_value) {
  ledcWrite(M2_IN_1_CHANNEL, pwm_value);
  ledcWrite(M2_IN_2_CHANNEL, 0);
}

void M1_stop() {
  ledcWrite(M1_IN_1_CHANNEL, PWM_MAX);
  ledcWrite(M1_IN_2_CHANNEL, PWM_MAX);
}
void M2_stop() {
  ledcWrite(M2_IN_1_CHANNEL, PWM_MAX);
  ledcWrite(M2_IN_2_CHANNEL, PWM_MAX);
}

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
  } else {
    Serial.println("Connection to server failed.");
    return;
  }
}

/*
 *  setup and loop
 */
void setup() {
  Serial.begin(115200);

  ledcSetup(M1_IN_1_CHANNEL, freq, resolution);
  ledcSetup(M1_IN_2_CHANNEL, freq, resolution);
  ledcSetup(M2_IN_1_CHANNEL, freq, resolution);
  ledcSetup(M2_IN_2_CHANNEL, freq, resolution);

  ledcAttachPin(M1_IN_1, M1_IN_1_CHANNEL);
  ledcAttachPin(M1_IN_2, M1_IN_2_CHANNEL);
  ledcAttachPin(M2_IN_1, M2_IN_1_CHANNEL);
  ledcAttachPin(M2_IN_2, M2_IN_2_CHANNEL);

  pinMode(M1_I_SENSE, INPUT);
  pinMode(M2_I_SENSE, INPUT);

  M1_stop();
  M2_stop();

  delay(100);
  client_setup();
}

void execute_cmd(char cmd) {
  if(cmd == 'w') {
    M1_forward(100);
    M2_forward(100);
  } else if (cmd == 's'){
    M1_backward(100);
    M2_backward(100);
  } else if(cmd == 'd'){
    M1_forward(100);
    M2_backward(100);
  } else if(cmd == 'a'){
    M2_forward(100);
    M1_backward(100);
  }
  delay(300);
  M1_stop();
  M2_stop();
}
void loop() {
  char cmd;
  if (!client.connected()) {
        //Serial.println("Disconnected from server. Attempting to reconnect...");
        // client.stop();
        delay(100); // Short delay before reconnecting
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
            char buffer[1];
            client.readBytes(buffer, 1); // Read data from server and unpack it
            Serial.printf("Received response - cmd %c", buffer[0]);
            cmd = buffer[0];
            execute_cmd(cmd);
        }
        //delay(5000); // Delay to avoid flooding server
    }
}
