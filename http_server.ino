// test connection - would be nice and handy to use a ntp server - needs some more research - just a quick proof of concept with a website
void server_connection_test ()
{
  byte ok;

  ok = connection_test ("google.com","/");

  if (ok)
  {
    heartbeat = millis () + heartbeatinterval;
    ser.print (F("server connection test ok="));
    ser.println (ok);
  }
  else
  {
//    heartbeat = millis () + 5000;  // debugging and comment next line for manual control
    server_down ();
    server_unresponsive_num++;
    ser.println (F("server connection test nok"));
  }
}

// server is down - main loop will handle a server restart
void server_down ()
{
  pinMode(led, OUTPUT);
  if (serverup==1)
    serverup=0;
  digitalWrite (led, LOW);
  serverreq=0;
  heartbeat = millis () + 1000;  // small delay before attempting to restart the server
}

// did we get a request? then we serve the webpage
void server_loop ()
{
  if ( serverup==1 )
  {
    if (serverreq==3)
    {
      switch (servertarget)
      {
        case ' ': webpage_brb (server_ch_id); break;
        case '_': serve_homepage_stats (server_ch_id); break;
        case 'A'...'N':
        case '1'...'6':
        case 'b': serve_homepage_boven (server_ch_id); break;
        case 'y': break;  // don't serve anything - for debugging
        default : webpage_notfound (server_ch_id); break;
      }
      serverreq=0;
    }

    // we test if the connection is working
    if ( (long)( millis () - heartbeat ) >= 0)
    {
      char buf[20];
      ser.print (F("Heartbeat ")); ser.println (readabletime_string(millis (),buf));
      heartbeat += heartbeatinterval;
      server_connection_test ();
    }
  }
  if (serverup==0)  // we ignore serverup==2
  {    // server not running?
    if ( (long)( millis () - heartbeat ) >= 0)
      server_start ();    // restart the server
  }
}

// we have a message from the esp
// we are looking for three strings, noting we got a request to show the page
void server_handle ()
{
  char *p;
  int packet_len;  // unused in this example

  if (serverup==1)
  {
    if (strncmp(espbuf, "+IPD,", 5)==0)  // outside world is calling
    {
      tictoc = millis ();
      requester=unknown;
      sscanf(espbuf+5, "%d,%d", &server_ch_id, &packet_len);  // we don't use the package_len
      if (server_ch_id_max < server_ch_id)                    // 5 connections possible - 0-4
        server_ch_id_max=server_ch_id;
      serverreq=1;
      last_connection=millis ();    // remember when was last activity
      heartbeat = millis () + heartbeatinterval;  // we reset the heartbeat for next connection-test
      p = strstr (espbuf, "GET /");
      if (p)                       // / followed by space = root; / followed by filename is a different page/file (closing char is space)
      {                            // space in a path is converted to %20 eg +IPD,1,374:GET /buttons%20.shtml HTTP/1.1\r\n
        serverreq=2;
        servertarget = *(p+5);     // we just need the first character of the target - keeping it simple
      }
    }
    if ( (serverreq == 2) )  // find out who is requesting ...
    {
      if (strstr (espbuf, "User-Agent:"))
      {
        if (strstr (espbuf, "iPhone"))
          requester=iphone;
        else
          if (strstr (espbuf, "Macintosh"))
            requester=mac;
          else
            requester=unknown;
      }
    }
    if (strncmp (espbuf, "OK\r\n",4)==0 && serverreq==2)
    {
      serverreq=3;
      serverreq_num++;
      last_connection=millis ();    // remember when was last activity
      heartbeat = millis () + heartbeatinterval;  // we reset the heartbeat for next connection-test
    }
  }
}

void get_ip ()
{
  char *p;

  esp.print(F("AT+CIFSR\r\n"));
  if ( waitforit (10000L,"192.168.1.") )
  {
    p = strstr (espbuf, "192.168.1.");
    sscanf ( (p+10), "%d", &ip);
    ser.print (F("ip found: "));
    ser.println (ip);
  }
  else
  {
    ser.print (F("stront aan de knikker"));  // geen ip = stront aan de knikker!
    ip=0;
  }
}

// we start the server with sequential commands
void server_start()
{
  byte ok;

  if ( (long) (millis() - heartbeat) >= 0)  // we use heartbeat for a small delay
  {

    // try empty AT command
    ok = espup ();
    if (ok)
    {
      esp.print(F("AT+CWMODE=1\r\n"));  // 1 means Station mode 2 means AP mode 3 means AP + Station mode
      ok=waitforit (5000L,"");
    }
    // older firmware may require a reset ?
  //  if (ok)
  //    ok = esp_reset ();
    if (ok)
    { 
      // join AP
      esp.print(F("AT+CWJAP=\""));  // join AP
      esp.print(SSID);
      esp.print(F("\",\""));
      esp.print(PASS);
      esp.print(F("\"\r\n"));
      ok = waitforit (30000L,"");
    }
    if (ok)
    { 
      // start server
      esp.print(F("AT+CIPMUX=1\r\n"));  // 0 single connection 1 multiple connection
      ok = waitforit(5000L,"");
    }
    if (ok)
    {
      get_ip ();

      esp.print(F("AT+CIPSERVER=1,")); // create server on port
      esp.print(ip+8080);
      esp.print (F("\r\n"));
      ok = waitforit (5000L,"");
    }
    if (ok)
    { 
      esp.print(F("AT+CIPSTO=120\n"));  // server timeout
      ok = waitforit (5000L,"");          // gave an error when done directly after the 'join AP' - notice only \n
    }
    if (ok)
    {
      ser.println (F("server running - try connecting to:"));
      ser.print (F("http://192.168.1."));
      ser.print (ip);
      ser.print (F(":"));
      ser.println (8080+ip);
      serverup=1;
      digitalWrite (led, HIGH);
      server_startok_num++;                // we count how many succesful starts
      heartbeat = millis () + 10000L;      // we force a connection check - just to be sure - after a small delay to let wifi settle
    }
    else
    {
      server_startnok_num++;
      ser.println (F("something went wrong :("));
      heartbeat = millis()+1000L;                  // don't retry to fast - give the esp8266 time to settle - otherwise you get busy reply
    }
  }
}

/*  if (ok)
  {
    esp.print(F("AT+CSYSWDTDISABLE\r\n"));  // disable watchdog every time???
    ok=waitforit (5000L,"");
    esp.print(F("ATE0\r\n"));  // disable echo every time?
    ok=waitforit (5000L,"");
  }
*/


