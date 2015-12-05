
/*
 *  Simple HTTP get webclient test
 *  With DS18B20 temperature reading
 *
 *  Board detail : https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts
 *  http://www.arduinesp.com/wifiwebserver
 *  http://iot-playground.com/2-uncategorised/41-esp8266-ds18b20-temperature-sensor-arduino-ide
 *
 *  Timer
 *  http://www.switchdoc.com/2015/10/iot-esp8266-timer-tutorial-arduino-ide/
 *  
 *  si ereur avec __ieee754_sqrt
 *  https://github.com/esp8266/Arduino/issues/612
 *  https://files.gitter.im/esp8266/Arduino/Abqa/libm.a.tbz replace the one in
 *  C:\Users\axel\AppData\Local\Arduino15\packages\esp8266\tools\xtensa-lx106-elf-gcc\1.20.0-26-gb404fb9\xtensa-lx106-elf\lib
 */

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

extern "C" {
#include "user_interface.h"
}


//Serial Config
#define BAUDRATE 115200  // Serial link speed

//Timer elements
os_timer_t myTimer1;
#define TIMER1 1000       //  1s for timer 1
os_timer_t myTimer2;
#define TIMER2 1000*60    // 60s for timer 2
os_timer_t myTimer3;
#define TIMER3 1000*60*15 //15mn for timer 3
// boolean to activate timer events
bool tick1Occured;
bool tick2Occured;
bool tick3Occured;
// Need to have a global address for the timer id
const char tickId1=1;
const char tickId2=2;
const char tickId3=3;
// Functions declaration
void timerCallback(void *);
void timerInit(os_timer_t *pTimerPointer, uint32_t milliSecondsValue, os_timer_func_t *pFunction, char *pId);


//Wifi config
const char* ssid     = "Wifi_du_9_av_Ginette";
const char* password = "bender le chat de ginette";
//Function declarations
void wifiConnect(void);
void webServerStart(void);
void ClientAction (void);

//Thingspeak config
String myWriteAPIKey = "WNZU5QA99N5HMB4O";
//Function declarations
void thingSpeakWrite (String, float, float, float, float, float, float, float, float);

//Sensors config
#define ONE_WIRE_BUS 14  // One wire pin for DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
//Function declarations
void sensorRead(void);


float temp;
float humidity;

#define BLUE_LED  2 // GPIO2 also used by wifi chip
#define RED_LED   0 // GPIO0
WiFiServer server(80);
WiFiClient client;


/* void setup(void) 
 *  Setup software run only once
 *  
*/
void setup() {
  
  //Start Serial
  Serial.begin(BAUDRATE);
  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("   ESP8266 Full Test      ");
  Serial.println("--------------------------");
 

  //Init and start timers
  tick1Occured = false;
  tick2Occured = false;
  tick3Occured = false;
  timerInit(&myTimer1, TIMER1,  timerCallback, (char*)&tickId1);
  timerInit(&myTimer2, TIMER2,  timerCallback, (char*)&tickId2);
  timerInit(&myTimer3, TIMER3,  timerCallback, (char*)&tickId3);

  //Init leds
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);

  //Init wifi and web server
  wifiConnect();
  webServerStart();

  //Init DHT temperature and Himidity reading
  dht.begin();
  
}


/* void loop(void) 
 *  Main program automatically loaded
 *  
*/
void loop() {


  // Check if a client has connected
  // Priority to the client to be more responsive
  WiFiClient client = server.available();
  if (client) {
    // Wait until the client sends some data
    Serial.println("new client");
    while(!client.available()){
      delay(1);
    }
    ClientAction();
  }

  //Check if a timer occured to execute the action
  //Timer 1 action
  if (tick1Occured == true){
    Serial.println("Tick 1s Occurred");
    tick1Occured = false;

    //Toggle LED
    digitalWrite(RED_LED, !digitalRead(RED_LED));
  }
  
  //Timer 2 action
  if (tick2Occured == true){
    Serial.println("Tick 1min Occurred");
    tick2Occured = false;
    
    //Temperature and Humidity Reading
    temp = dht.readHumidity();
    humidity = dht.readTemperature();
    thingSpeakWrite (myWriteAPIKey, temp, humidity, NAN, NAN, NAN, NAN, NAN, NAN);

    Serial.print("Temperature DHT22 : ");
    Serial.print(t);
    Serial.print(" - Humidity: ");
    Serial.print(h);
  }
  
  //Timer 3 action
  if (tick3Occured == true){
    Serial.println("Tick 15m Occurred");
    tick3Occured = false;
  }
  
  //Give th time to th os to do his things
  yield();  // or delay(0);
}


/* void timerCallback(void *pArg)
 *  Function 
 *  
 *  Input  : 
 *  Output :
*/
void timerCallback(void *pArg) {
  
  char timerId = *(char*)pArg; //Value inside (*) of pArg, casted into a char pointer
  
  switch (timerId){
    case 1 :
      tick1Occured = true;
      break;
    case 2 :
      tick2Occured = true;
      break;
    case 3 :
      tick3Occured = true;
      break;
    default :
      //Nothings to do
      break;
  }
} 

/* timerInit(os_timer_t *pTimerPointer, uint32_t milliSecondsValue, os_timer_func_t *pFunction, char *pId) 
 *  Function 
 *  
 *  Input  : 
 *  Output :
*/
void timerInit(os_timer_t *pTimerPointer, uint32_t milliSecondsValue, os_timer_func_t *pFunction, char *pId) {
   /*
    Maximum 7 timers
    os_timer_setfn - Define a function to be called when the timer fires
    void os_timer_setfn(os_timer_t *pTimer, os_timer_func_t *pFunction, void *pArg)
    
    Define the callback function that will be called when the timer reaches zero. 
    The pTimer parameters is a pointer to the timer control structure.
    The pFunction parameters is a pointer to the callback function.
    The pArg parameter is a value that will be passed into the called back function. 
    The callback function should have the signature: void (*functionName)(void *pArg)
    The pArg parameter is the value registered with the callback function.
  */
  os_timer_setfn(pTimerPointer, pFunction, pId);
  /*
    os_timer_arm -  Enable a millisecond granularity timer.
    void os_timer_arm( os_timer_t *pTimer, uint32_t milliseconds, bool repeat)
  
    Arm a timer such that is starts ticking and fires when the clock reaches zero.
    The pTimer parameter is a pointed to a timer control structure.
    The milliseconds parameter is the duration of the timer measured in milliseconds. 
    The repeat parameter is whether or not the timer will restart once it has reached zero.
  */
  os_timer_arm(pTimerPointer, milliSecondsValue, true);
} 

/* void wifiConnect(void)
 *  Function 
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

/* void webServerStart(void)
 *  Function 
 *  
 *  Input  : 
 *  Output :
*/
void webServerStart(void){
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void ClientAction (void)
{
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(BLUE_LED, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(BLUE_LED, LOW);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 2 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 2 OFF<br>");
  client.print("The temperature is ");
  client.print(temp);
  client.println("<br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}


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


