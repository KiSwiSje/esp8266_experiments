#include "string.h"
#include "arduino.h"
#include "printfthingie.h"
#include <avr/pgmspace.h>

const byte led=13;

#define ESPBUF_SIZE 256  // can probably be a smaller
#define SERBUF_SIZE 80

#define SSID  "syslink"           // change this to match your WiFi SSID
#define PASS  "hpm68ftevc8y7bws"  // change this to match your WiFi password
#define PORT  "8080"              // using port 8080 by default

char espbuf[ESPBUF_SIZE];
char serbuf[SERBUF_SIZE];

int espPos=0;
int serPos=0;

byte esp_msg=0;

byte esp_rst=0;
long esp_restart_num=0;

// for the server example
byte serverup=0; // 2: debug mode, will not restart, will not respond 1: server will respond; 0: server will be restarted automatically
byte serverreq=0;
long serverreq_num=0;
long server_startok_num=0;
long server_startnok_num=0;
int server_ch_id;
char servertarget;

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
      case '\n':   // end of text
        serbuf [serPos] = 0;  // terminating byte
        ser_handle (serbuf);
        serPos = 0;  
      break;    
      default:
        if (serPos < (SERBUF_SIZE - 1))
          serbuf [serPos++] = c;
        else
          ; //error - character ignored, did not fit in the buffer
      break;
    }  // switch
  }
}

// adds characters to the espbuf, echo character on serial
void esp_listen ()
{
  if (esp.available () > 0)
  {
    char c = esp.read ();
    esp_show_char (c);        // echo character on serial IMMEDIATELY
    espbuf [espPos++] = c;
    if ( (espPos >= (ESPBUF_SIZE-1)) || (c == '\n') || ((c == ' ') && (espbuf[0] == '>') && (espPos == 2)) )
    {  // \n OR > followed by space at the beginning of a line closes the espbuf - or buffer full
      espbuf [espPos] = 0;  // terminating byte AFTER the closing character
      esp_msg=1;            // we have a message from the esp
    }
  }
}

void ser_handle (const char * data)
{
ser.print (F("ser:"));
ser.println (serbuf);  // echo command

    switch (data[0])
    {
      int i;
      unsigned long baud;

      case '_':  // prefix to run commands
        if ( (data[1] == 0) || (strcmp (&data[1], "stats") == 0) )      // _ or _stats gives some stats
          ser_stats ();
        if (strcmp (&data[1], "version") == 0)                          // example of a shortcut - version of the esp firmware
          esp.print (F("AT+GMR\r\n"));
        if (strcmp (&data[1], "connect") == 0)                          // example of a shortcut - connect to a wifi network with encryption
          esp.print (F("AT+CWJAP=\"syslink\",\"hpm68ftevc8y7bws\"\r\n"));  // this does not check the reply for success
        if (strcmp (&data[1], "espup") == 0)
          if (espup ())
            ser.println (F("esp8266 responsive"));
          else
            ser.println (F("esp8266 not responsive"));
        if (strcmp (&data[1], "reset") == 0)
          esp_reset ();
        if (strncmp (&data[1], "baud",4) == 0)
        {
          sscanf(&data[5], "%lu", &baud);
          ser.print (F("baud:")); ser.println (baud);
          esp.end ();
          esp.begin (baud);
        }
        if (strcmp (&data[1], "server") == 0)
          server_start ();
        if (strcmp (&data[1], "stop") == 0)          // for testing purposes
        {
          if (serverup==1)
          {
            serverup=2;                            // we stop responding to server requests
            digitalWrite (led, LOW);
          }
          else
          {
            serverup=1;
            digitalWrite (led, HIGH);
          }
          
          ser.print (F("serverup:")); ser.println (serverup);
        }

      break;

      case 'A':  // we assume this is an AT command - so we pass it to the esp8266
        for (i=0; i<strlen(data); i++)
          esp.print (data[i]);
        esp.print ("\r\n");          // send \r\n ; older versions of the firmware only want \n
      break;
      case '-':
        for (i=1; i<strlen(data); i++)
          esp.print (data[i]);
        esp.print ("\n");          // only send \n
      break;
      case '+':
        for (i=1; i<strlen(data); i++)
          esp.print (data[i]);
        esp.print ("\r\n");          // send \r\n
      break;

      default:  // we assume this is a text command (in response to the > prompt)
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
    case '\0':  // closing byte of a string - will never be displayed in this context!!!
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
  if ( strstr(espbuf, "ai-thinker.com Version:0.9.2.4") )  // for some reason the esp restarted
    esp_rst=1;
  if ( (strncmp (espbuf, "ready\r\n", 7)==0 || (strncmp (espbuf, "Ready\r\n", 7)==0))  && esp_rst==1)                    // restart complete
  {
    ser.println (F("**************************************************************************************"));
    ser.println (F("**************************************************************************************"));
    ser.println (F("****************************** esp restarted *****************************************"));
    ser.println (F("**************************************************************************************"));
    ser.println (F("**************************************************************************************"));

    esp_restart_num++;
    ser_stats ();

    esp_rst=0;
    server_down ();  // server is down
   }

  espPos = 0;
  esp_msg=0;
}

void setup ()
{
  printf_begin();

  esp.begin(57600);  // older firmware has different baud rate
  ser.begin(57600);
  ser.println (F("esp8266_experiments - auto starting the server"));

  server_down ();
}

void loop ()
{
  ser_listen ();
  esp_listen ();
  if (esp_msg)
  {
    server_handle (); // scan the incoming messages
    esp_handle ();    // checks if the esp restarted - resets esp_msg to 0
  }
  server_loop ();     // handle webpage requests
  if (serverup==0)      // server not running?
  {
    delay(1000);
    server_start ();  // restart the server
  }
}




