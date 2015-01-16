/* ====== esp8266 experiments ======
 *
 * (Updated Dec 23, 2014)
 *
 * 
 * connect with http://192.168.1.102:8080 to the server
 *

serial commands:
starting with _    : commands
starting with A    : AT commands - passed to the esp8266 and closed with \r\n
starting with -    : everything after the - is passed and closed with \n
all the rest       : passed to the esp8266 and closed with \n

_ or _stats        : show some server related stats
_connect           : connect to the wifi router
_start             : start the server
_stop              : stops automatic responses to server requests - for manual intervention
_pause             : pause the server / disable auto-restart
_version           : version command
_espup             : simple AT command to check esp8266 is responsive
_reset             : reset command, restarting the esp8266
                   : note - Ian (check sources) has a good idea, tie the ch_pd connection to a arduino pin to control hardware resetting
_baud              : set baudrate to talk to esp8266
_test              : test the connection
_chunksize         : set the chunksize

connections:

atmega2560
esp8266, 3,3V, CD4050 for protecting the rx/tx from 5v
esp firmware 0018000902-AI03 - [Vendor:www.ai-thinker.com Version:0.9.2.4]  (my esp came standard like this)
serial = usb
serial1 goes to the esp8266
serial monitor 57600 baud, newline only

atmega328p with software serial: untested

to do:
*******

after AT+CIFSR - check ip - and set the 8080 accordingly so servers on different ip's are accessible from the outside

if unresponsive for some time - try resetting the esp8266 with reset command / hard reset?
handle the buttons in the boven page
auto baudrate ???

some features in my firmware:
http://www.electrodragon.com/w/Wi07c#Firmware
AT+CIOBAUD=?       // inquiry
AT+CIOBAUD?        // check
AT+CIOBAUD=57600   // Set was succesful first time - lets see if it can handle multiple connection now
AT+CSYSWDTDISABLE  // don't see change - more testing needed
AT+CSYSWDTENABLE


build new webpage_stats
build webpage_b
test on 328p
there are three softwareserial librarys
softwareserial (original)
newsoftwareserial (merged into softwareserial original?)
altsoftwareserial (better latency) https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

log power outage periods?
some buttons, and how to check they were pressed




sources:
Davew: (on the arduino.cc forum)
The simple way to see what's send back is to telnet (use putty) to the <ipaddress> and <port> and type the 'GET / HTTP/1.1' 

Nick Gammon's site is a source of information, in this example, methods for reading serial streams inspired me, also, freeram
http://www.gammon.com.au/serial

Ray Wang @ Rayshobby LLC
http://rayshobby.net/?p=9734

maniacbug - nanode example
http://maniacbug.wordpress.com/2012/01/28/webserver/

Ian Sexton
http://www.cse.dmu.ac.uk/~sexton/esp8266/

find string in a string - needle in the haystack - strstr ()
http://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
eg.
   char haystack[20] = "TutorialsQoint234";
   char needle[10] = "Point";
   char *ret;

   ret = strstr(haystack, needle);

   if (ret)
     printf("The substring is: %s\n", ret);
   else
     printf ("not found");

*/

