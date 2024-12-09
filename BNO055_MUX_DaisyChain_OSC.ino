#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>

char ssid[] = "Wifi_Name";      // your network SSID (name)
char pass[] = "Wifi_password";  // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(192, 168, 0, 102);  // remote IP 
const unsigned int outPort = 9999;        // remote port 

// All the BNO055 Sensors that we want to use: We can put two Sensors in a row one with the I²C address 0x28 the other one 0x29 (solderd Adr Pin)
// first value is an id, I am adding the address to the id for easier debugging
Adafruit_BNO055 bno128 = Adafruit_BNO055(128, 0x28);
Adafruit_BNO055 bno129 = Adafruit_BNO055(129, 0x29);
Adafruit_BNO055 bno228 = Adafruit_BNO055(228, 0x28);
Adafruit_BNO055 bno229 = Adafruit_BNO055(229, 0x29);
Adafruit_BNO055 bno328 = Adafruit_BNO055(328, 0x28);
Adafruit_BNO055 bno329 = Adafruit_BNO055(329, 0x29);
Adafruit_BNO055 bno428 = Adafruit_BNO055(429, 0x28);
Adafruit_BNO055 bno429 = Adafruit_BNO055(429, 0x29);
Adafruit_BNO055 bno528 = Adafruit_BNO055(528, 0x28);
Adafruit_BNO055 bno529 = Adafruit_BNO055(529, 0x29);
//put all sensors in an array for easier looping
Adafruit_BNO055 bno[] = { bno128, bno129, bno228, bno229, bno328, bno329, bno428, bno429, bno528, bno529 };

QWIICMUX myMux;

#define NUMBER_OF_SENSORS 10

// in port order you can define the ports you want to use
int portOrder[] = { 0, 2, 3, 4, 7 };
int portNum = 0;
bool firstSensor = true;

// the sensor names hold the OSC addresses, note the port order that you are using
char* sensorNames[10] = { "/upperarm.l", "/lowerarm.l", "/body", "/head", "/upperleg.l", "/lowerleg.l", "/upperleg.r", "/lowerleg.r", "/upperarm.r", "/lowerarm.r" };


void setup() {
  Serial.begin(115200);
  Serial.print("Setup: ");
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //Initialize Mux
  Serial.println("Qwiic Mux Shield Read Example");
  Wire.begin();
  if (myMux.begin() == false) {
    Serial.println("Mux not detected. Freezing...");
    while (1)
      ;
  }
  Serial.println("Mux detected");

  //Initialize BNOs in Ports
  bool initSuccess = true;
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    myMux.setPort(portOrder[portNum]);
    String portString = String(portOrder[portNum]);
    if (!bno[i].begin()) {
      // There was a problem detecting the BNO055 ... check your connections
      Serial.print(i);
      Serial.print("at Port" + portString);
      Serial.print(bno[initSuccess].getMode());
      Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      while (1)
        ;
      initSuccess = false;
    } else {
      delay(100);
      //Here we set the axis remap, for more details see the document
      bno[i].setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_PUPPET_BODY);
      bno[i].setAxisSign(Adafruit_BNO055::REMAP_SIGN_P4);
      //setting mode to NDOF, which uses all sensors on the BNO055 and fuses the data
      bno[i].setMode(OPERATION_MODE_NDOF);
      delay(100);
      bno[i].setExtCrystalUse(true);
      Serial.print(i);
      Serial.print(" at Port" + portString);
      Serial.println(" configured");
    }
    if (i % 2 != 0) {
      portNum++;
    }
    delay(100);
  }
  delay(100);
  firstSensor = true;
  portNum = 0;

  if (initSuccess == false) {
    Serial.print("Freezing...");
    while (1)
      ;
  }
  firstSensor = true;
  portNum = 0;
}

void loop() {


  //loop through all ports and sensors
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    myMux.setPort(portOrder[portNum]);
    imu::Quaternion quat;
    uint8_t sys, gyro, accel, mag = 0;
    //store the quaternion from the sensor
    quat = bno[i].getQuat();
    bno[i].getCalibration(&sys, &gyro, &accel, &mag);

    OSCMessage msg(sensorNames[i]);

    uint8_t calibration = sys + gyro + accel + mag;

    //add the individual quaternion values to the osc message
    float w = quat.w();
    float x = quat.x();
    float y = quat.y();
    float z = quat.z();
    msg.add(w);
    msg.add(x);
    msg.add(y);
    msg.add(z);

    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);    // Send the bytes to the SLIP stream
    Udp.endPacket();  // Mark the end of the OSC Packet
    msg.empty();

    if (i % 2 != 0) {
      portNum++;
    }
  }
  firstSensor = true;
  portNum = 0;
}
