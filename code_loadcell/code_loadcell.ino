//-------------------------------------------------------------------------------------
// Esp32 master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Tested with      : HX711 asian module on channel A and YZC-133 20kg load cell.
// Tested with MCU  : Esp32
//-------------------------------------------------------------------------------------

#include <HX711_ADC.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h> 

//Set wifi for esp
const char* ssid = "Connectify-Biet_kich_my";
const char* password =  "gacon637651";
// The information and credentials of the MQTT server.
// The server address, the port, the username and the password.
const char* mqttServer = "m12.cloudmqtt.com";
const int mqttPort = 18893;
const char* mqttUser = "arzobkwy";
const char* mqttPassword = "n8-rY11a0Z2L";

//The constructor of this class can receive other number of arguments,
//as can be seen in the API documentation.
WiFiClient espClient;
PubSubClient client(espClient);
// U8x8 Contructor List 
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 
//Pin Esp32 connect with Hx711
HX711_ADC LoadCell(4,16);

long t;

void setup() {
   Serial.begin(115200);
 //Line 40 to line 48 set wifi for esp using smart config
   WiFi.mode(WIFI_AP_STA);
   WiFi.beginSmartConfig();
   while (!WiFi.smartConfigDone()) {
   delay(500);
   Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");
  //Line 50 to 67 start by connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000); 
    }
  } 
  // Start oled
     u8g2.begin();
     u8g2.enableUTF8Print();

 //Pin Esp connect gas sensor
     pinMode(5,INPUT_PULLUP);
  
  Serial.println("Wait...");

  //start loadcell
  LoadCell.begin();
  // tare preciscion can be improved by adding a few seconds of stabilising time
  long stabilisingtime = 2000; 
  LoadCell.start(stabilisingtime);
  // user set calibration factor (float)
  LoadCell.setCalFactor(100000); 
  Serial.println("Startup + tare is complete");
}

void loop() {
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in scetch will reduce effective sample rate (be carefull with delay() in loop)
  LoadCell.update();

  //get smoothed value from data set + current calibration factor
  if (millis() > t + 250) {

// get value from loadcell
    float i = LoadCell.getData();
   i= fabs(i);

   // create string to send to server and show in oled
    String payload = "";
    payload += i;
    payload += "kg \n";

    //show the message if the quantity is smaller than 0.5kg
    if((i < 0.5) && (i>0.05))
    {
      payload += "WARNING";
    }
    Serial.print("Load_cell output val: ");
    Serial.println(i);
    t = millis();
  // clear the internal memory
   u8g2.clearBuffer(); 
   // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall        
   u8g2.setFont(u8g2_font_7x13B_mr ); 
    // write something to the internal memory 
   u8g2.drawStr(1,18,"val:");
   //  sets the cursor location
   u8g2.setCursor(28, 18);
   //prints the variable in parentheses.
   u8g2.print(i);
   // write something to the internal memory 
   u8g2.drawStr(65,18,"kg");  
   // publish a message on a topic "test_demo/load"
  client.publish("test_demo/load", (char*) payload.c_str());

  // check status gas sensor and publish to server 
  if(digitalRead(5)==0)
  {
     u8g2.drawStr(1,32,"GAS");
     client.publish("test_demo/test", "GAS");
    }
    else
    {
      u8g2.drawStr(1,32,"NO GAS");
      client.publish("test_demo/test", "NO GAS");
      }
   //transfer internal memory to the display 
   u8g2.sendBuffer(); 
  }
  //reset value loadcell
  if(digitalRead(2)==1)
  {

   LoadCell.start(100); 
 
  }
  else
  { 
  }
  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  //check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

}
