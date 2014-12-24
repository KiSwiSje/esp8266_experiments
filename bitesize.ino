
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
    }
  }
  return (found);
}

// we check if the esp8266 is responding with the simple 'AT' command - we want to receive 'OK'
byte espup ()
{
  byte found;

  esp.print ("AT\r\n");
  found = waitforit (5000L);
  return (found);
}

// this needs further testing!!! some weirdness, but works
byte esp_reset ()
{
  byte found=0;

  esp.print("AT+RST\r\n");
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
  ser.println ("statistics:");
  ser.print ("# restarts detected:"); ser.println (esp_rst_num);
  ser.print ("server running:"); ser.println (serverup);
  ser.print ("server requests received:");  ser.println (serverreq_num);
}


// http://www.gammon.com.au/forum/?id=12615
int freeRam ()
{
  return (RAMEND - size_t (__malloc_heap_start));
}

