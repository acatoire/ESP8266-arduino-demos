
/*
 *  Simple implementation of 2 One wire DS18B20 Sensor reading logged on Thingspeaks
 *  
 *  This program simply wait for the nex sensor read, no timer, no fancy things
 *
 *  Developed from :
 *    http://iot-playground.com/2-uncategorised/41-esp8266-ds18b20-temperature-sensor-arduino-ide
 *  Use the librairies
 *  - OneWire
 *  - DallasTemperature
 *  
 */

//Include credential files
#include "wifi_log.h"
#include "thingspeak_log.h"

//Librairies include
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Serial Config
#define BAUDRATE 115200  // Serial link speed

//Wifi config
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
WiFiClient client; // Global wifi client
//Wifi Function declarations
void wifiConnect(void);
void ClientAction (void);

//Thingspeak config
String myWriteAPIKey = TS_WRITE_KEY;
//Thingspeak Function declarations
void thingSpeakWrite (String, float, float, float, float, float, float, float, float);
                      
//Sensors config
#define ONE_WIRE_BUS 14  // One wire pin for DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
//Sensors Function declarations
void sensorRead(void);

//Sensors reading frequency
#define FREQ 1000*60*15 //15mn for slow monitoring
//#define FREQ 1000*20 // 20s for test

//Temperature saved values
float temp1;
float temp2;

// LED pin definition for HUZZAH board
#define BLUE_LED  2 // GPIO2 also used by wifi chip
#define RED_LED   0 // GPIO0


/* void setup(void) 
 *  Setup software run only once
 *  
*/
void setup() {
  
  //Start Serial
  Serial.begin(BAUDRATE);
  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("   ESP8266 Temp Test");
  Serial.println("--------------------------");

  //Init leds
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

  //Init wifi
  wifiConnect();
 
}

/* void loop(void) 
 *  Main program automatically loaded
 *  
*/
void loop() 
{
  //Turn ON LED
  digitalWrite(RED_LED, LOW);
  //Read sensors values
  sensorRead();
  
  //Turn OFF LED
  digitalWrite(RED_LED, HIGH);

  //Sent data on Thingspeak.com
  thingSpeakWrite (   myWriteAPIKey,
                      temp1, temp2, NAN, NAN,
                      NAN, NAN, NAN, NAN);

  //Write temp on serial link
  Serial.print("Temperature DS18B20 : ");
  Serial.print(temp1);
  Serial.print(" ");
  Serial.println(temp2);
  
  //Wait before next measurment
  //Thingspeak accepte one write every 15s max
  delay(FREQ);
}

/* void wifiConnect(void)
 *  Function to connect the board to the wifi
 *  
 *  Input  : None
 *  Output : None
*/
void wifiConnect(void){
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
}

 /* void sensorRead(void)
 *  Get the sensors value from One Wire bus
 *  
 *  Input  : None
 *  Output : None
*/
void sensorRead(void) {

  //DS18B20 Reading
  DS18B20.requestTemperatures(); 
  temp1 = DS18B20.getTempCByIndex(0);
  temp1 = round(temp1*10)/10.0;        //Use 10.0 not 10 to stay in float
  temp2 = DS18B20.getTempCByIndex(1);
  temp2 = round(temp2*10)/10.0;
}


 /* void thingSpeakWrite (String APIKey,
                          float field1, float field2, float field3, float field4,
                          float field5, float field6, float field7, float field8)
 *  Save value on thingspeak.com servers
 *  
 *  Input  : APIKey the write api key for the desired chanel
 *           fieldx value to save, use NAN if the field isn't use.
 *  Output : None
*/
void thingSpeakWrite (String APIKey,
                      float field1, float field2, float field3, float field4,
                      float field5, float field6, float field7, float field8)
{
  
  const char* thingspeakServer = "api.thingspeak.com";

  if (client.connect(thingspeakServer,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = APIKey;
    if (!isnan(field1))
    {
      postStr +="&field1=";
      postStr += String(field1);
    }
    if (!isnan(field2))
    {
      postStr +="&field2=";
      postStr += String(field2);
    }
    if (!isnan(field3))
    {
      postStr +="&field3=";
      postStr += String(field3);
    }
    if (!isnan(field4))
    {
      postStr +="&field4=";
      postStr += String(field4);
    }
    if (!isnan(field5))
    {
      postStr +="&field5=";
      postStr += String(field5);
    }
    if (!isnan(field6))
    {
      postStr +="&field6=";
      postStr += String(field6);
    }
    if (!isnan(field7))
    {
      postStr +="&field7=";
      postStr += String(field7);
    }
    if (!isnan(field8))
    {
      postStr +="&field8=";
      postStr += String(field8);
    }
    postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+myWriteAPIKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
 
  }
  client.stop();
}




