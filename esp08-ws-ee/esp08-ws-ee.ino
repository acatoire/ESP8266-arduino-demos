
/*
 *  Simple implementation of a web server with parametrable wifi credentials
 *    Wifi SSID and PASS can be change with serial commande: 
 *      PXXXXXXX - For new password
 *      SXXXXXXX - For new ssid
 *    Wifi SSID and PASS are saved in EEPROM
 *  
 *  This program simply wait for the next client connexion, no timer, no fancy things
 *
 *  Developed from :
 *    http://www.arduinesp.com/wifiwebserver
 *    https://github.com/sandeepmistry/esp8266-Arduino/tree/master/esp8266com/esp8266/libraries/EEPROM
 *  Use the librairies
 *  - 
 *  
 */


#include <ESP8266WiFi.h>
#include <EEPROM.h>

//Include credential files

//Serial Config
#define BAUDRATE 115200  // Serial link speed

//Wifi config
char ssid[32];
char password[32];
//Wifi Function declarations
void wifiConnect(void);
void webServerStart(void);
void ClientAction (void);
//Wifi Global variables
WiFiServer server(80);
WiFiClient client;

//EEPROM config
#define eeAddrSSID 0 // address in the EEPROM  for SSID
#define eeSizeSSID 32 // size in the EEPROM  for SSID
#define eeAddrPASS 32 // address in the EEPROM  for SSID
#define eeSizePASS 32 // size in the EEPROM  for SSID
//EEPROM Function declarations
void eepromWrite(void);
void eepromRead(void);
//EEPROM Global variables
bool needSave;

//LED pin definition for HUZZAH board
#define BLUE_LED  2 // GPIO2 also used by wifi chip
#define RED_LED   0 // GPIO0

// Serial string management variables
void executeCommand();
void serialStack();
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

 
void setup() {
  
  //Start Serial
  Serial.begin(BAUDRATE);

  //Init and turn off leds
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

  eepromRead();
 
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

  
  if (needSave){
    needSave = 0;
    eepromWrite();
  }

  //Check for serial input
  if(Serial.available())
  {
    serialStack();
  }
  //Execute recived command
  if (stringComplete)
  {
    executeCommand();
    inputString = "";
    stringComplete = false;
  }
 
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

/* void eepromWrite(void)
 *  Function 
 *  
 *  Input  : 
 *  Output :
*/
void eepromWrite(void)
{
  char letter;
  int i, addr;
  //Activate eeprom
  EEPROM.begin(512);

  Serial.println("Save new wifi credential");
  Serial.print(ssid);
  Serial.print(' ');
  Serial.println(password);

  // advance to the next address.  there are 512 bytes in
  // the EEPROM, so go back to 0 when we hit 512.
  // save all changes to the flash.
  addr = eeAddrSSID;
  for (i = 0 ; i < eeSizeSSID ; i++)
  { 
    EEPROM.write(addr, ssid[i]);
    if('\0' == ssid[i])
      break;
    addr++;
  }
  // advance to the next address.  there are 512 bytes in
  // the EEPROM, so go back to 0 when we hit 512.
  // save all changes to the flash.
  addr = eeAddrPASS;
  for (i = 0 ; i < eeSizePASS ; i++)
  {
    EEPROM.write(addr, password[i]);
    if('\0' == password[i])
      break;
    addr++;
  }
  EEPROM.end();
}

/* void eepromWrite(void)
 *  Function 
 *  
 *  Input  : 
 *  Output :
*/
void eepromRead(void)
{
  char letter;
  int i, addr;
  //Activate eeprom
  EEPROM.begin(512);

  // advance to the next address.  there are 512 bytes in
  // the EEPROM, so go back to 0 when we hit 512.
  // save all changes to the flash.
  addr = eeAddrSSID;
  for (i = 0 ; i < eeSizeSSID ; i++)
  { 
    ssid[i] = EEPROM.read(addr);
    if('\0' == ssid[i])
      break;
    addr++;
  }
  
  // advance to the next address.  there are 512 bytes in
  // the EEPROM, so go back to 0 when we hit 512.
  // save all changes to the flash.
  addr = eeAddrPASS;
  for (i = 0 ; i < eeSizePASS ; i++)
  {
    password[i] = EEPROM.read(addr);
    if('\0' == password[i])
      break;
    addr++;
  }

  Serial.println("New wifi credential read");
  Serial.print(ssid);
  Serial.print(' ');
  Serial.println(password);
}

/* void serialStack(void)
 *  Stack all serial char received in one string until a '\n'
 *  Set stringComplete to True when '\n' is received
 *  This implementation is good enough for this project because serial commands
 *  will be send slowly.
*/
void serialStack()
{
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    
    // if the incoming character is a newline, set a flag
    if (inChar == '\n') 
    {// so the main loop can do something about it
      inputString += '\0';
      stringComplete = true;
    }
    else
    {// add it to the inputString
      inputString += inChar;
    }
  }
}

/* void executeCommand(void)
 *  Execute received serial command
 *  Commandes list :
 *   - "i", "I", "info", "Info" : Return basic system informations, mainly for debug purpose
 *   - "FXX"                    : Setup the minimum temperature delta to activate the Airflow (0 to 100 C)
 *   - "fXX"                    : Setup the maximum temperature delta to disable the Airflow (0 to 100 C)
 *   - "S1" or "S0"             : Enable ("S1") or disable ("S0") the summer mode
 *
*/
void executeCommand()
{
  // Define the appropriate action depending on the first character of the string
  switch (inputString[0]) 
  {
    // INFO Request
    case 'i':
    case 'I':
      //printInfo(); // Print on serial the system info
      Serial.print("Wifi SSID:");
      Serial.print(ssid);
      Serial.print(" - Pass:");
      Serial.println(password);
      break;
    // PASSWORD
    case 'P':
      inputString.remove(0,1);
      Serial.print("P > Change the wifi pasword to : ");
      Serial.println(inputString);
      strcpy (password, inputString.c_str());
      break;
    // SSID
    case 'S':
      inputString.remove(0,1);
      Serial.print("S > Change the SSID to : ");
      Serial.println(inputString);
      strcpy (ssid, inputString.c_str());
      break;
    // Unknown command 
    default: 
      Serial.print(inputString[0]);
      Serial.println(" > ?");
  }
}

//TODO read ssid ans password from serial
