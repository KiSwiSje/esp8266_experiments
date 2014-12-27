
// this is generic - lets hope esp messages are consistent for different commands?
// this method blocks
// returns 1 if we got a positive response
//         0 if we got a negative response, or timed out
byte waitforit (long timeout)
{
  byte found=0;

  unsigned long t = millis () + timeout;

  while ( ( (long)(millis () - t) < 0) && !found)
  {
    esp_listen ();
    if (esp_msg)
    {
      esp_handle ();                                   // handles esp restart if necessary
      if ( strncmp(espbuf, "OK\r\n",4)==0 )
        found=1;
      else
        if ( strncmp(espbuf, "no change\r\n",11)==0 )  // AT+CWMODE=# can return this
          found=1;
        else
          if ( strncmp (espbuf, "> ",2)==0 )          // cipsend returns this - asking input
            found=1;
          else
            if ( strncmp(espbuf, "ERROR\r\n",7)==0 )
              t -= timeout;                            // to force exiting the while loop
            else
              if ( strncmp(espbuf, "FAIL\r\n",6)==0 )
                t -= timeout;                            // to force exiting the while loop
    }
  }
  return (found);
}

// we check if the esp8266 is responding with the simple 'AT' command - we want to receive 'OK'
byte espup ()
{
  byte found;

  esp.print (F("AT\r\n"));
  found = waitforit (5000L);
  return (found);
}

// this needs further testing!!! some weirdness, but works
byte esp_reset ()
{
  byte found=0;

  esp.print(F("AT+RST\r\n"));
  unsigned long t = millis () + 10000;

  while ( ( (long)(millis () - t) < 0) && found != 2)
  {
    esp_listen ();
    if (esp_msg)
    {
      esp_handle ();            // catches the restart - will restart the server if it was running - show esp messages
      if ( strstr(espbuf, "[Vendor:www.ai-thinker.com Version:0.9.2.4]\r\n") )
        found++;
      else
        if ( strncmp(espbuf, "ready\r\n",7)==0 )
          found++;
    }
  }
  return (found==2);
}

// show some statistics on the serial port
void ser_stats ()
{
  char buf[20];

  ser.println (F("statistics:"));
  ser.print (F("esp starts:")); ser.println (esp_restart_num);
  ser.print (F("server running:")); ser.println (serverup);
  ser.print (F("server requests received:"));  ser.println (serverreq_num);
  ser.print (F("server start ok:"));  ser.println (server_startok_num);
  ser.print (F("server start nok:"));  ser.println (server_startnok_num);
  ser.print (F("Uptime(s):"));  ser.println (readabletime_string(millis (),buf));
}

char* readabletime_string (unsigned long time, char *buf)
{
  long temp=time;
  byte bufindex=0;

  if (temp >= (24*3600000L))
  {
    sprintf (&buf[bufindex], "%dd", temp / (24*3600000L));
    bufindex += strlen (buf);
    temp %= (24*3600000L);
  }

  if (temp >= 3600000L)
  {
    sprintf (&buf[bufindex], "%02dh", temp / 3600000L);
    bufindex += 3;
    temp %= 3600000L;
  }
  sprintf (&buf[bufindex], "%02dm", temp / 60000L);
  temp %= 60000L;
  bufindex += 3;
  sprintf (&buf[bufindex], "%02ds", temp / 1000L);
  return (buf);
}

// http://www.gammon.com.au/forum/?id=12615
int freeRam ()
{
  return (RAMEND - size_t (__malloc_heap_start));
}


