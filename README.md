# ESP8266-arduino-demos
Batch of demo code for the ESP8266 chip with arduino IDE

# Needed material and librairies
Tested on adafruit HUZZAH : https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts
With ARDUINO 1.6.6 with esp8266 1.6.5 package. 
I actually have issue with version 2.0.0

# Naming convention
 *  espXX - Base number
 *  wr    - Include a web request to get data from a website
 *  ws    - Include a web server
 *  ts    - use thingspeak.com
 *  dht   - read temperature and humidity on a DHT sensor
 *  tds   - read temperature on a DS18B20 sensor
 *  tm    - use of os_timer capability
 *  ee    - use of EEPROM

# Possibles bugs
IF you encounter an error with __ieee754_sqrt :
The issue is known here:
 *  https://github.com/esp8266/Arduino/issues/612
The solution is to download 
 *  https://files.gitter.im/esp8266/Arduino/Abqa/libm.a.tbz
and replace the one in
 *  C:\Users\xxxxx\AppData\Local\Arduino15\packages\esp8266\tools\xtensa-lx106-elf-gcc\1.20.0-26-gb404fb9\xtensa-lx106-elf\lib
