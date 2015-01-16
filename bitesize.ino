
// this is generic - lets hope esp messages are consistent for different commands?
// this method blocks
// returns 1 if we got a positive response
//         0 if we got a negative response, or timed out
// specific string reply overrides other 'positive' replys
byte waitforit (long timeout, const char asc[])
{
  byte found=0;

  unsigned long t = millis () + timeout;

  while ( ( (long)(millis () - t) < 0) && !found)
  {
    esp_listen ();
    if (esp_msg)
    {
      esp_handle ();                                   // handles esp restart if necessary

      // positive reply?
      if (asc[0] != 0)  // we search a specific reply
      {
        if ( strncmp(espbuf, asc,strlen(asc))==0 )      // only this string is a good reply
          found=1;    // eg. server connection test - returns OK and then Linked
      }
      else  // we test for generic positive reply
      {
        if ( strncmp(espbuf, "OK\r\n",4)==0 )
          found=1;
        else
          if ( strncmp(espbuf, "no change\r\n",11)==0 )  // AT+CWMODE=# can return this
            found=1;
          else
            if ( strncmp (espbuf, "> ",2)==0 )          // cipsend returns this - asking input
              found=1;
      }

      // negative reply?
      if (! found)
        if ( strncmp(espbuf, "ERROR\r\n",7)==0 )
          t -= timeout;                            // to force exiting the while loop
        else
          if ( strncmp(espbuf, "FAIL\r\n",6)==0 )
            t -= timeout;
          else
            if ( strncmp(espbuf, "DNS Fail\r\n",10)==0 )  // server connection test bad reply
              t -= timeout;
    }
  }
  return (found);
}

/*
debugging:
http://www.msftncsi.com/  - en of andere portscanner? korte pagina met gewoon text "Page not found" - gebruiken voor connection test?
AT+CIPSTART=4,"TCP","google.com",80
AT+CIPSTART=4,"TCP","macfan.nl",80
AT+CIPSEND=4,14
GET / HTTP/1.1
AT+CIPCLOSE=4
*/
byte connection_test (const char server[],const char page[])  // http://www.msftncsi.com/ncsi.txt - die pagina toevoegen aan get /ncsi.txt
{
  String request;
  byte ok=0;

  request="GET ";
  request += page;
  request += " HTTP/1.1";
  
  ok = espup ();

  if (ok)
  {
    esp.print(F("AT+CIPCLOSE=4\r\n"));  // we don't care about the reply - we just want to avoid "ALREAY CONNECT\r\n" reply to cipstart
    waitforit (1000,"");

    esp.print(F("AT+CIPSTART=4,\"TCP\",\""));
    esp.print (server);
    esp.print (F("\",80\r\n"));  // actually - we want OK\r\n and then Linked\r\n - this times out after a while?
    ok = waitforit (5000L,"");            // we want OK\r\n - "ALREAY CONNECT\r\n" also possible
  }
  if (ok)
    ok = waitforit (5000L,"Linked\r\n");  // and then Linked\r\n
  if (ok)
  {
    esp.print(F("AT+CIPSEND=4,"));
    esp.print (request.length());
    esp.print (F("\r\n"));
    waitforit (5000L,"");                  // we want > space
  }
  if (ok)
  {
    esp.print(request);  // we want OK\r\n twice !!! and then the unlink??? maybe if we just let that time out?
    ok += waitforit (5000L,"");         // one OK\r\n
  }
  if (ok)
    ok += waitforit (5000L,"");  // second OK\r\n
  esp.print(F("AT+CIPCLOSE=4\r\n"));  // OK\r\n followed by Unlink\r\n      OR          link is not\r\n
  waitforit (1000,"");
  return (ok);
}

// we check if the esp8266 is responding with the simple 'AT' command - we want to receive 'OK'
byte espup ()
{
  byte found;

  esp.print (F("AT\r\n"));
  found = waitforit (5000L,"");
  return (found);
}

// this needs further testing!! some weirdness, but works
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
  ser.print (F("ip:192.168.1."));  ser.print (ip); ser.print (F(":"));  ser.println (8080+ip);
  ser.print (F("Uptime(s):"));  ser.println (readabletime_string(millis (),buf));

  ser.print (F("esp starts:")); ser.println (esp_restart_num);
  ser.print (F("last restart:")); ser.print (readabletime_string(millis()-last_esp_restart,buf)); ser.println (F(" ago"));

  ser.print (F("server running:")); ser.println (serverup);
  ser.print (F("pages served:"));  ser.println (serverreq_num);
  ser.print (F("last connection:"));  ser.print (readabletime_string(millis()-last_connection,buf)); ser.println (F(" ago"));
  ser.print (F("server start ok:"));  ser.println (server_startok_num);
  ser.print (F("server start nok:"));  ser.println (server_startnok_num);
  ser.print (F("connection unresponsive:"));  ser.println (server_unresponsive_num);
  ser.print (F("chunksize:"));  ser.println (chunksize);
  ser.print (F("server_ch_id_max:"));  ser.println (server_ch_id_max);
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


