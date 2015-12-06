
/*
 *  Simple implementation of a web server
 *  
 *  This program simply wait for the next client connexion, no timer, no fancy things
 *
 *  Developed from :
 *    http://www.arduinesp.com/wifiwebserver
 *  Use the librairies
 *  - 
 *  
 */


#include <ESP8266WiFi.h>

//Include credential files
#include "wifi_log.h"

//Serial Config
#define BAUDRATE 115200  // Serial link speed

//Wifi config
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
//Wifi Function declarations
void wifiConnect(void);
void webServerStart(void);
void ClientAction (void);
//Wifi Global variables
WiFiServer server(80);
WiFiClient client;
 
//LED pin definition for HUZZAH board
#define BLUE_LED  2 // GPIO2 also used by wifi chip
#define RED_LED   0 // GPIO0
 
void setup() {
  
  //Start Serial
  Serial.begin(BAUDRATE);

  //Init and turn off leds
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, HIGH);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  //Init wifi and web server
  wifiConnect();
  webServerStart();
 
}
 
void loop() {
  
  // Check if a client has connected
  client = server.available();
  ClientAction();
 
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

/* void ClientAction(void)
 *  Function 
 *  
 *  Input  : 
 *  Output :
*/
void ClientAction() {
  // Check if a client has connected
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
    Serial.print(".");
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 

  if (request.indexOf("/BLUE_LED=ON") != -1)  {
    digitalWrite(BLUE_LED, LOW);
  }
  if (request.indexOf("/BLUE_LED=OFF") != -1)  {
    digitalWrite(BLUE_LED, HIGH);
  }

  if (request.indexOf("/RED_LED=ON") != -1)  {
    digitalWrite(RED_LED, LOW);
  }
  if (request.indexOf("/RED_LED=OFF") != -1)  {
    digitalWrite(RED_LED, HIGH);
  }
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<br><br>");
  
  if(digitalRead(BLUE_LED)) { // HIGH is OFF
    client.println("Click <a href=\"/BLUE_LED=ON\">here</a> turn ON the BLUE LED<br>");
  } else {
    client.println("Click <a href=\"/BLUE_LED=OFF\">here</a> turn OFF the BLUE LED<br>");
  }

  if(digitalRead(RED_LED)) { // HIGH is OFF
    client.println("Click <a href=\"/RED_LED=ON\">here</a> turn ON the RED LED<br>");
  } else {
    client.println("Click <a href=\"/RED_LED=OFF\">here</a> turn OFF the RED LED<br>");
  }
  client.println("<br><br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

