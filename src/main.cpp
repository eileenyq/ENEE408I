#include <Arduino.h>
#include <Adafruit_MCP3008.h>
#include <Encoder.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ADC (line sensor)
Adafruit_MCP3008 adc1;
Adafruit_MCP3008 adc2;

const unsigned int ADC_1_CS = 2;
const unsigned int ADC_2_CS = 17;

int adc1_buf[8];
int adc2_buf[8];

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

Adafruit_MPU6050 mpu;

// PID
const int base_pid = 80; // Base speed for robot
const float mid = 5.33;

float e;
float d_e;
float total_e;
float prev_e;

// Assign values to the following feedback constants:
float Kp = 5;
float Kd = 5;
float Ki = .0001;

/*
 *  Line sensor functions
 */
void digitalConvert();
float getRightMostPosition();
void readADC() {
  for (int i = 0; i < 8; i++) {
    adc1_buf[i] = adc1.readADC(i); //read from one side
    adc2_buf[i] = adc2.readADC(i);
  }
}

void rotateNDegrees2(int degrees) {
  float angleZ = 0;
  unsigned long previousTime = millis();

  //digitalWrite(M1_IN_1, HIGH);
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, 100);
  ledcWrite(M2_IN_1_CHANNEL, 100);
  ledcWrite(M2_IN_2_CHANNEL, 0);
  while (abs(angleZ) < degrees) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;

    angleZ += g.gyro.z * elapsedTime * (180.0 / PI);

    //Serial.print("Angle: ");
    //Serial.println(angleZ);

    previousTime = currentTime;
    delay(10);
  }
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, 0);
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, 0);
  Serial.print(degrees);
  Serial.println("-degree rotation complete!");
}
void rotateNDegrees1(int degrees) {
  float angleZ = 0;
  unsigned long previousTime = millis();

  ledcWrite(M1_IN_1_CHANNEL, 100);
  ledcWrite(M1_IN_2_CHANNEL, 0);
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, 100);
  while (abs(angleZ) < degrees) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;

    angleZ += g.gyro.z * elapsedTime * (180.0 / PI);

    // Serial.print("Angle: ");
    // Serial.println(angleZ);

    previousTime = currentTime;
    delay(10);
  }
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, 0);
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, 0);
  Serial.print(degrees);
  Serial.println("-degree rotation complete!");
}

void rotateClockwise(int degrees) {
  printf("rotatingClockwise %d degrees\n", degrees);
  float angleZ = 0;
  unsigned long previousTime = millis();
  int u =0;
  int counter = 0;
  while (true) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;

    angleZ += g.gyro.z * elapsedTime * (180.0 / PI);

    previousTime = currentTime;
    u = 90;
    //Serial.printf("[rotateClockwise()] u: %d\n", u);
    if (degrees + angleZ < -0.5){
      ledcWrite(M1_IN_1_CHANNEL, 0);//
      ledcWrite(M1_IN_2_CHANNEL, 0);
      ledcWrite(M2_IN_1_CHANNEL, 0);
      ledcWrite(M2_IN_2_CHANNEL, u);//
    } else if (degrees + angleZ > 0.5) {
      ledcWrite(M1_IN_1_CHANNEL, 0);
      ledcWrite(M1_IN_2_CHANNEL, u);//
      ledcWrite(M2_IN_1_CHANNEL, 0);//
      ledcWrite(M2_IN_2_CHANNEL, 0);
    } else {
      counter++;
      if (counter == 3){
        Serial.print("breaking");
        break;
      }
    }
    delay(10);
  }
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, 0);
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, 0);
  Serial.print(degrees);
  Serial.println("-degree rotation complete!");
  delay(1000);
}
int getLastWhite(){
  for (int i=1; i<13; i++){
    if (lineArray[i]==1){
      return i - 1;
    }
  }
  return 13;
}
void rotateBox() {
  int u =0;
  int counter = 0;
  while (true) {
    readADC();
    digitalConvert();
    int pos = getLastWhite();
    u = 80;
    if (6 - pos > 0){
      ledcWrite(M1_IN_1_CHANNEL, 0);
      ledcWrite(M1_IN_2_CHANNEL, u);
      ledcWrite(M2_IN_1_CHANNEL, 0);//
      ledcWrite(M2_IN_2_CHANNEL, 0);
    } else if (6 - pos < 0) {
      ledcWrite(M1_IN_1_CHANNEL, 0);//
      ledcWrite(M1_IN_2_CHANNEL, 0);
      ledcWrite(M2_IN_1_CHANNEL, 0);
      ledcWrite(M2_IN_2_CHANNEL, u);
    } else {
      break;
    }
    delay(10);
  }
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, 0);
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, 0);
  delay(1000);
}

void rotateCounterClockwise(int degrees) {
  float angleZ = 0;
  unsigned long previousTime = millis();
  int u =0;
  int counter = 0;
  while (true) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;

    angleZ += g.gyro.z * elapsedTime * (180.0 / PI);

    previousTime = currentTime;
    u = 90;
    Serial.println(u);
    if (degrees - angleZ < -0.5){
      ledcWrite(M1_IN_1_CHANNEL, 0);
      ledcWrite(M1_IN_2_CHANNEL, u);
      ledcWrite(M2_IN_1_CHANNEL, 0);//
      ledcWrite(M2_IN_2_CHANNEL, 0);
    } else if (degrees - angleZ > 0.5) {
      ledcWrite(M1_IN_1_CHANNEL, 0);//
      ledcWrite(M1_IN_2_CHANNEL, 0);
      ledcWrite(M2_IN_1_CHANNEL, 0);
      ledcWrite(M2_IN_2_CHANNEL, u);
    } else {
      Serial.print("breaking");
      counter++;
      if (counter == 2){
        break;
      }
    }
    delay(10);
  }
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, 0);
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, 0);
  Serial.print(degrees);
  Serial.println("-degree rotation complete!");
  delay(1000);
}

// Converts ADC readings to binary array lineArray[] (Check threshold for your robot)
void digitalConvert() {
  int threshold = 700;
  for (int i = 0; i < 7; i++) {
    if (adc1_buf[i]>threshold) {
      lineArray[2*i] = 0;
    } else {
      lineArray[2*i] = 1;
    }
    if (i<6) {
      if (adc2_buf[i]>threshold){
        lineArray[2*i+1] = 0;
      } else {
        lineArray[2*i+1] = 1;
      }
    }
  }
  for (int i=0; i<13; i++){
    Serial.print(lineArray[i]);
  }
  Serial.println();
}
// Calculate robot's position on the line
float getPosition(/* Arguments */) {
  int position = 6;
  /* Using lineArray[], which is an array of 13 Boolean values representing 1
   * if the line sensor reads a white surface and 0 for a dark surface,
   * this function returns a value between 0-12 for where the sensor thinks
   * the center of line is (6 being the middle)
   */
  readADC();
  digitalConvert();
  int sum = 0;
  int numBlck = 0;
  for(int i = 0; i < 12; i++) {
    if (lineArray[i] == 0) {
      sum += i;
      numBlck++;
    }
  }

  if (numBlck == 0) {
    return (float)5.33;
  } else if (numBlck >= 10) {
    return (float)5.33;
  } else {
    return (float)sum/(float)numBlck;
  }
}

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

void jumpForward(int distance){
  float angleZ = 0;
  unsigned long previousTime = millis();
  int u = 0;
  float kp = 1;
  float ki = 0.01;
  float kd = 20;
  int rightWheelPWM = 0;
  int leftWheelPWM = 0;
  int total = 0;
  for (int i=0; i<distance; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;
    angleZ += g.gyro.z * elapsedTime * (180.0 / PI);
    previousTime = currentTime;
    delay(10);
    int lastAngle;
    int diff = angleZ-lastAngle;
    total = total + angleZ;
    u = kp*angleZ + kd*diff+ ki*total;
    lastAngle = angleZ;
    rightWheelPWM = 100 - u;
    leftWheelPWM = 100 + u;
    M1_forward(leftWheelPWM);
    M2_forward(rightWheelPWM);
  }
  M1_stop();
  M2_stop();
}

void jumpBackward(int distance){
  float angleZ = 0;
  unsigned long previousTime = millis();
  float kp = 2;
  float ki = 0.01;
  float kd = 10;
  int u = 0, rightWheelPWM = 0, leftWheelPWM = 0, total = 0;
  for (int i = 0; i < distance; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;
    angleZ += g.gyro.z * elapsedTime * (180.0 / PI);
    previousTime = currentTime;
    delay(10);
    int lastAngle;
    int diff = angleZ-lastAngle;
    total = total + angleZ;
    u = kp*angleZ + kd*diff+ ki*total;
    lastAngle = angleZ;
    rightWheelPWM = 100 - u;
    leftWheelPWM = 100 + u;
    M1_backward(rightWheelPWM);
    M2_backward(leftWheelPWM);
  }
  M1_stop();
  M2_stop();
}

bool allBlack(){
  for (int i = 0; i < 13; i++) {
    if (lineArray[i] != 0) {
      return false;
    }
  }
  return true;
}

bool allWhite() {
  for (int i = 0; i < 13; i++) {
    if (lineArray[i] != 1) {
      return false;
    }
  }
  return true;
}

int check(){
  jumpForward(25);
  delay(5000);
  readADC();
  digitalConvert();
  jumpBackward(25);
  if (allBlack()){
    return 1;
  } else if (allWhite()){
    return 2;
  } else {
    return 3;
  }
}

int isCorner() {
  readADC();
  digitalConvert();
  int c;
  Serial.print("isCorner(): ");
  if (allWhite()){
    c = check();
    if (c == 1){
      Serial.println("T Shape");
      return 2;
    } else if (c == 2){
      Serial.println("White Box");
      return 3;
    } else {
      Serial.println("Plus Shape");
      return 4;
    }
  } else if (lineArray[8] == 1 && lineArray[9] == 1 && lineArray[10] == 1 && lineArray[11] == 1 && lineArray[12] == 1) {
    c = check();
    if (c == 1){
      Serial.println("Left Corner");
      return 5;
    } else if (c == 2){
      Serial.println("IDK");
      return 6;
    } else {
      Serial.println("Left/Straight");
      return 7;
    }
  } else if (lineArray[4] == 1 && lineArray[3] == 1 && lineArray[2] == 1 && lineArray[1] == 1 && lineArray[0] == 1) {
    c = check();
    if (c == 1){
      Serial.println("Right Corner");
      return 8;
    } else if (c == 2){
      Serial.println("IDK");
      return 9;
    } else {
      Serial.println("Right/Straight");
      return 10;
    }
  } else {
    Serial.println("Undefined Line Status");
    delay(1000);
  }
  return 1;
}

void turnCorner(int direction) {
  /*
   * Use the encoder readings to turn the robot 90 degrees clockwise or
   * counterclockwise depending on the argument. You can calculate when the
   * robot has turned 90 degrees using either the IMU or the encoders + wheel measurements
   */
  //direction = 1 -> turn right, direction = 0 -> turn left
  if (direction == 1) {
    M1_forward(80);
    M2_backward(80);
    //check robot position
  } else {
    M1_backward(80);
    M2_forward(80);
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

  adc1.begin(ADC_1_CS);
  adc2.begin(ADC_2_CS);

  pinMode(M1_I_SENSE, INPUT);
  pinMode(M2_I_SENSE, INPUT);

  M1_stop();
  M2_stop();

  delay(100);

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
}

float getRightMostPosition(){
  for (int i=0; i<13; i++){
    if (lineArray[i]==1){
      return i;
    }
  }
  return 0;
}

void followBox() {
  //this code follows along the box counter clockwise
  Serial.printf("In followBox()\n");
  float ki = 0;
  float kp = 5;
  float kd = 10;
  float pos;
  int u;
  float e;
  float prev_e;
  readADC();
  digitalConvert();
  // jumpForward(10);
  // rotateClockwise(90);
  while (!allBlack()){
    readADC();
    digitalConvert();
    if (allWhite()) {
      M1_stop();
      M2_stop();
      delay(5000);
      return;
    }
    pos = getRightMostPosition();
    e = 7 - pos;
    u = kp*e + kd*(e - prev_e);
    // Serial.print("pos: ");
    // Serial.print(pos);
    // Serial.print("  u: ");
    // Serial.println(u);
    prev_e = e;
    if (u > 100){
      u = 100;
    }
    // if (allWhite()){
    //   Serial.print("All White\n");
    // }
    ledcWrite(M1_IN_1_CHANNEL, 0);
    ledcWrite(M1_IN_2_CHANNEL, 80 + u);
    ledcWrite(M2_IN_1_CHANNEL, 0);
    ledcWrite(M2_IN_2_CHANNEL, 80 - u);
  }
  M1_stop();
  M2_stop();
  delay(2000);
  //rotateClockwise(90);
  //rotateCounterClockwise(90);
  rotateBox();
  followBox();
}

int isEdge(){
  readADC();
  digitalConvert();
  if(allBlack() || allWhite()) {
    return 0;
  }
  //Are we on a White | Black edge?
  int i = 0;
  while(i < 13 && lineArray[i] == 0) {
    i++;
  }
  while(i < 13 && lineArray[i] == 1) {
    i++;
  }
  if (i != 13) {
    return 0;
  } else {
    return 1;
  }
}
void loop() {
  prev_e = 0;
  int u = 0, rightWheelPWM = 0, leftWheelPWM = 0, basePWM = 80, lineStatus = 0;
  float pos = 0;

  while(true) {
    readADC();
    digitalConvert();
    delay(1);
    pos = getPosition();
    //Serial.printf("pos: %.2f", pos);
    //Define the PID errors
    e = mid - pos;
    d_e = (e - prev_e);
    total_e = total_e + e;
    prev_e = e;
    //Serial.printf(", e: %.2f, prev_e: %.2f, d_e: %.2f, total_e: %.2f, u: %d, ", e, prev_e, d_e, total_e, u);
    //Implement PID control (include safeguards for when the PWM values go below 0 or exceed maximum)
    u = Kp*e + Ki*total_e + Kd*d_e;

    //Serial.printf(", rightWheelPWM: %d, leftWheelPWM: %d\n", rightWheelPWM, leftWheelPWM);

    //lineStatus = isCorner();
    Serial.printf("Pos: %d\n\n\n", getLastWhite());
    if (allWhite()){
      rotateClockwise(90);
      delay(1000);
    } else if (isEdge()){
      followBox();
    } else {
      if (basePWM -u < 0|| basePWM + u > 255 || pos == 13) {
        rightWheelPWM = 0;
        leftWheelPWM = 0;
      } else {
        rightWheelPWM = basePWM + u; //positive error
        leftWheelPWM = basePWM - u;
      }
      M1_forward(leftWheelPWM);
      M2_forward(rightWheelPWM);
    }

    // Check for corners
    // int side = isCorner();
    // Serial.print("side");
    // Serial.print(side);
    // if(side == 1) {
    //   M1_forward(150);
    //   M2_forward(150);
    //   delay(500);
    //   rotateNDegrees1(60);
    //   delay(500);
    // } else if (side == 2) {
    //   M1_forward(150);
    //   M2_forward(150);
    //   delay(500);
    //   rotateNDegrees2(60);
    //   delay(500);
    // } else if (side == 3) {
    //   //all white
    //   delay(1000);
    //   M1_forward(150);
    //   M2_forward(150);
    //   delay(1000);
    //   readADC();
    //   digitalConvert();
    //   if (allWhite()) {
    //     //still all white, we turn
    //     rotateNDegrees2(60); //turns right
    //     //check if we are in box
    //     delay(1000);
    //     followBox();
    //   } else {
    //     rotateNDegrees2(60);
    //   }
    //   delay(500);
    // } else if(side == 4) {
    //   //all black
    //   delay(1000);
    //   M1_forward(150);
    //   M2_forward(150);
    //   delay(1000);
    //   rotateNDegrees2(60);
    //   delay(2000);
    // } else {
    // if (basePWM -u < 0|| basePWM + u > 255 || pos == 13) {
    //   rightWheelPWM = 0;
    //   leftWheelPWM = 0;
    // } else {
    //   rightWheelPWM = basePWM + u; //positive error
    //   leftWheelPWM = basePWM - u;
    // }
    // M1_forward(leftWheelPWM);
    // M2_forward(rightWheelPWM);
  }
}
