/* ====== ESP8266 experiments ======
 *
 * (Updated Dec 21, 2014)
 *
 * based on Ray Wang's sketch
 * =================================
 *
 * Change SSID and PASS to match your WiFi settings.
 * The IP address is displayed to soft serial upon successful connection.
 *
 * Ray Wang @ Rayshobby LLC
 * http://rayshobby.net/?p=9734
 *
 * 
 * tested on mega2560
 * serial monitor nl/57600
 * esp firmware 0018000902-AI03 - [Vendor:www.ai-thinker.com Version:0.9.2.4]  (my esp came standard like this)
 * connected with http://192.168.1.102:8080
 */

#include "string.h"
#include "arduino.h"
#include "printfthingie.h"

#define ESPBUF_SIZE 512
#define SERBUF_SIZE 80

#define SSID  "syslink"      // change this to match your WiFi SSID
#define PASS  "hpm68ftevc8y7bws"  // change this to match your WiFi password
#define PORT  "8080"           // using port 8080 by default

char espbuf[ESPBUF_SIZE];
char serbuf[SERBUF_SIZE];

int espPos=0;
int serPos=0;

byte esp_msg=0;

byte esp_rst=0;
long esp_rst_num=0;

// for the server example
byte serverup=0;
byte serverreq=0;
long serverreq_num=0;
int ch_id;

/*
// If using Software Serial for debug
// Use the definitions below
//#include <SoftwareSerial.h>
//SoftwareSerial ser(7,8);  // use pins 7, 8 for software serial 
//#define esp Serial
*/

// If your MCU has dual USARTs (e.g. ATmega644, 1280, 2560)
// Use the definitions below
#define ser Serial    // use Serial for monitoring
#define esp Serial1   // use Serial1 to talk to esp8266

void ser_listen ()
{
  char c;

  if (ser.available () > 0)
  {
    c = ser.read ();
    switch (c)
    {
//      case ' ':
//      case '\t':
      case '\n':   // end of text
        serbuf [serPos] = 0;  // terminating byte
        ser_handle (serbuf);
        serPos = 0;  
      break;    
      default:
        if (serPos < (SERBUF_SIZE - 1))
          serbuf [serPos++] = c;
        else
          ; //error(6001);  // character ignored, did not fit in the buffer
      break;
    }  // switch
  }
}

// adds characters to the espbuf, shows character for character on serial
void esp_listen ()
{
  if (esp.available () > 0)
  {
    char c = esp.read ();

    if (espPos < (ESPBUF_SIZE - 2))
    {
      espbuf [espPos++] = c;
      esp_show_char (c);
      if ( (c == '\n') || ((c == ' ') && (espbuf[0] == '>') && (espPos == 2)) )    // \n OR > followed by space at the beginning of a line closes the espbuf
      {
        espbuf [espPos] = 0;  // terminating byte AFTER the closing character
        esp_msg=1;            // we have a message from the esp
      }
    }
    else
      ser.println ("******* error character ignored, did not fit in the buffer ********");
  }
}

// _ is the prefix to run certain commands
// AT commands are (almost) always closed with \r\n (for older version, this is \n)
// - prefix = some commands are closed with \n
void ser_handle (const char * data)
{
    ser.print ("ser:");
    ser.println (serbuf);  // echo command

    switch (data[0])
    {
      int i;

      case '_':  // prefix to run commands
        if ( (data[1] == 0) || (strcmp (&data[1], "stats") == 0) )      // _ or _stats gives some stats
          stats ();
        if (strcmp (&data[1], "version") == 0)                          // example of a shortcut - version of the esp firmware
          esp.print ("AT+GMR\r\n");
        if (strcmp (&data[1], "connect") == 0)                          // example of a shortcut - connect to a wifi network with encryption
          esp.print ("AT+CWJAP=\"syslink\",\"hpm68ftevc8y7bws\"\r\n");  // this does not check the reply for success
        if (strcmp (&data[1], "espup") == 0)
          if (espup ())
            ser.println ("esp8266 responsive");
          else
            ser.println ("esp8266 not responsive");
        if (strcmp (&data[1], "reset") == 0)
          esp_reset ();
        if (strcmp (&data[1], "server") == 0)
          server_start ();
        if (strcmp (&data[1], "stop") == 0)
          serverup=0;

      break;

      case 'A':  // we assume this is an AT command - so we pass it to the esp8266
        for (i=0; i<strlen(data); i++)
          esp.print (data[i]);
        esp.print ("\r\n");          // older versions of the firmware only want \n
      break;
      case '-':  // we assume this is an AT command - so we pass it to the esp8266
        for (i=1; i<strlen(data); i++)
          esp.print (data[i]);
        esp.print ("\n");          // only send \n
      break;

      default:  // we assume this is a text command
        for (i=0; i<strlen(data); i++)
          esp.print (data[i]);
        esp.print ("\n");          // only \n
      break;
    }
}  // end of ser_handle

// this visualizes hidden characters like newline and carriage return
void esp_show_char (const char data)
{
  switch (data)
  {
    case '\0':  // closing byte of a string - can never be displayed in this context!!!
      ser.print ("\\0");
    break;
    case '\a':  // 7 bel
      ser.print ("\\a");
    break;
    case '\b':  // 8 backspace
      ser.print ("\\b");
    break;
    case '\t':  // 9 tab
      ser.print ("\\t");
    break;
    case '\n':  // 10 newline
      ser.println ("\\n");
    break;
    case 11:  // 11 vertical tab
      ser.print ("\\vt");
    break;
    case '\f':  // 12 formfeed
      ser.print ("\\f");
    break;
    case '\r':  // 13 carriage return
      ser.print ("\\r");
    break;
    default:
      ser.print (data);
    break;
  }
}

void esp_handle ()
{
  if (serverup)
    server_handle ();

  if ( strstr(espbuf, "[Vendor:www.ai-thinker.com Version:0.9.2.4]\r\n") )  // for some reason the esp restarted
    esp_rst=1;
  if (strncmp (espbuf, "ready\r\n", 7)==0 && esp_rst==1)                    // restart complete
  {
    ser.println ("**************************************************************************************");
    ser.println ("**************************************************************************************");
    ser.println ("****************************** esp restarted *****************************************");
    ser.println ("**************************************************************************************");
    ser.println ("**************************************************************************************");

    if (serverup)                   // the server was running
    {
      esp_rst_num++;  // we count how many restarts and server_restarts we detected
      stats ();       // for serial monitoring
      serverup=0;
      serverreq=0;
      server_start ();              // restart it
    }
    esp_rst=0;
  }

  espPos = 0;
  esp_msg=0;
}

void setup ()
{
  printf_begin();
  esp.begin(9600);
  ser.begin(57600);
  ser.println ("Hello world - esp8266_experiments");
}

void loop ()
{
  ser_listen ();
  esp_listen ();
  if (esp_msg)
    esp_handle ();
  if ( serverup && (serverreq==3) )
  {
    serve_stats (ch_id);
    serverreq=0;
  }
}




