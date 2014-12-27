
// server is down - main loop will handle a server restart
void server_down ()
{
  pinMode(led, OUTPUT);
  if (serverup==1)
    serverup=0;
  digitalWrite (led, LOW);
  serverreq=0;
}

// did we get a request? then we serve the webpage
void server_loop ()
{
  if ( serverup==1 && (serverreq==3) )
  {
    switch (servertarget)
    {
      case ' ': webpage_brb (server_ch_id); break;
      case '9': serve_homepage_stats (server_ch_id); break;
      default : webpage_notfound (server_ch_id); break;
    }
    serverreq=0;
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
    if (strncmp(espbuf, "+IPD,", 5)==0)
    {
      sscanf(espbuf+5, "%d,%d", &server_ch_id, &packet_len);  // we don't use the package_len
      serverreq=1;
    }
    p = strstr (espbuf, "GET /");
    if ( p && serverreq==1)      // / followed by space = root; / followed by filename is a different page/file (closing char is space)
    {                            // space in a path is converted to %20 eg +IPD,1,374:GET /buttons%20.shtml HTTP/1.1\r\n
      serverreq=2;
      servertarget = *(p+5);      // we just need the first character of the target - keeping it simple
ser.print (F("***********")); ser.println (servertarget);
    }
    if (strncmp (espbuf, "OK\r\n",4)==0 && serverreq==2)
    {
      serverreq=3;
      serverreq_num++;
    }
  }
}

// we start the server with sequential commands
void server_start()
{
  byte ok;

  // try empty AT command
  ok = espup ();
  if (ok)
  {
    esp.print(F("AT+CWMODE=1\r\n"));
    ok=waitforit (5000L);
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
    ok = waitforit (30000L);
  }
  if (ok)
  { 
    // start server
    esp.print(F("AT+CIPMUX=1\r\n"));
    ok = waitforit(5000L);
  }
  if (ok)
  {
    esp.print(F("AT+CIPSERVER=1,")); // turn on TCP service
    esp.print(PORT);
    esp.print (F("\r\n"));
    ok = waitforit (5000L);
  }
  if (ok)
  { 
    esp.print(F("AT+CIPSTO=30\n"));  // gave an error when done directly after the 'join AP'
    ok = waitforit (5000L);
  }
  if (ok)
  {
    ser.println (F("server running - try connecting to:"));
    ser.println (F("http://ip:8080"));
    serverup=1;
    digitalWrite (led, HIGH);
    server_startok_num++;                // we count how many succesful starts
    ser.print(F("device ip addr:"));
    esp.print(F("AT+CIFSR\r\n"));
  }
  else
  {
    server_startnok_num++;
    ser.println (F("something went wrong :("));
  }
}

/*  if (ok)
  {
    esp.print(F("AT+CSYSWDTDISABLE\r\n"));  // disable watchdog every time???
    ok=waitforit (5000L);
  }
*/


