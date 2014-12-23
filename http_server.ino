void server_start()
{
  byte ok;

  // try empty AT command
  ok = espup ();
  if (ok) //ser.println("ok"); else ser.println ("nok");
  {
    esp.print("AT+CWMODE=1\r\n");
    ok=waitforit (5000L);
  }
  // newer firmware does not require a reset anymore ?
//  if (ok) //ser.println("ok"); else ser.println ("nok");
//    ok = esp_reset ();
  if (ok) //ser.println("ok"); else ser.println ("nok");
  { 
    // join AP
    esp.print("AT+CWJAP=\"");
    esp.print(SSID);
    esp.print("\",\"");
    esp.print(PASS);
    esp.print("\"\r\n");
    ok = waitforit (30000L);
  }
  if (ok) //ser.println("ok"); else ser.println ("nok");
  { 
    // start server
    esp.print("AT+CIPMUX=1\r\n");
    ok = waitforit(5000L);
  }
  if (ok) //ser.println("ok"); else ser.println ("nok");
  {
    esp.print("AT+CIPSERVER=1,"); // turn on TCP service
    esp.print(PORT);
    esp.print ("\r\n");
    ok = waitforit (5000L);
  }
  if (ok) //ser.println("ok"); else ser.println ("nok");
  { 
    esp.print("AT+CIPSTO=30\n");  // gave an error when done directly after the 'join AP'
    ok = waitforit (5000L);
  }
  if (ok) //ser.println("ok"); else ser.println ("nok");
  {
    ser.println ("server running - try connecting to:");
    ser.println ("http://ip:8080");
    serverup=1;
    ser.print("device ip addr:");
    esp.print("AT+CIFSR\r\n");
  }
  else
    ser.println ("something went wrong :(");
}

// we are looking for three strings, noting we got a request to show the page
void server_handle ()
{
  int packet_len;  // unused in this example

  if (serverup)
  {
    if (strncmp(espbuf, "+IPD,", 5)==0)
    {
      sscanf(espbuf+5, "%d,%d", &ch_id, &packet_len);  // we don't use the package_len
      serverreq=1;
    }
    if (strstr (espbuf, "GET /") && serverreq==1)      // / followed by space = root; / followed by filename is a different page/file (closing char is space
      serverreq=2;                                     // space in a path is converted to %20 eg +IPD,1,374:GET /buttons%20.shtml HTTP/1.1\r\n
    if (strncmp (espbuf, "OK\r\n",4)==0 && serverreq==2)
    {
      serverreq=3;
      serverreq_num++;
    }
  }
}

void serve_homepage_ray(int ch_id) {
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 10\r\n";

  String content="";
  // output the value of each analog input pin
  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
    int sensorReading = analogRead(analogChannel);
    content += "analog input ";
    content += analogChannel;
    content += " is ";
    content += sensorReading;
    content += "<br />\n";       
  }

  header += "Content-Length:";
  header += (int)(content.length());
  header += "\r\n\r\n";
  esp.print("AT+CIPSEND=");
  esp.print(ch_id);
  esp.print(",");
  esp.println(header.length()+content.length());
  if (waitforit (1000))  // we wait for the "> "
  {
    esp.print(header);
    esp.print(content);
  }
}


void serve_notavail(int ch_id) {
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 10\r\n";
  String content = "<html><head><title>Service Temporarily Unavailable</title></head><body><h3>This service is currently \"unavailable\"</h3><p><em>The main server is currently off-line.<br />Please try again later.</em></p></body></html>\r\n";

  header += "Content-Length:";
  header += (int)(content.length());
  header += "\r\n\r\n";

  esp.print("AT+CIPSEND=");
  esp.print(ch_id);
  esp.print(",");
  esp.println(header.length()+content.length());
  esp.print(header);
  esp.print(content);
}

void serve_node(int ch_id)
{
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 10\r\n";

  String content = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">";

  content += "<html>";
  content += "<head>";
  content += "<title>fuckedup!!</title>";
  content += "<meta name=\"viewport\" content=\"width=device-width, minimum-scale=1, maximum-scale=1, initial-scale=1\">";
  content += "<link rel=\"stylesheet\" href=\"http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css\" type=\"text/css\">";
  content += "<script src=\"http://code.jquery.com/jquery-1.6.4.min.js\" type=\"text/javascript\"></script>";
  content += "<script src=\"http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js\" type=\"text/javascript\"></script>";
  content += "<script src=\"/index.js\" type=\"text/javascript\"></script>";
  content += "<style type=\"text/css\">div.ui-block-b { text-align:right; }</style>";

  String content2 = "</head>";
  content2 += "<body>";
  content2 += "<div id=\"home\" data-role=\"page\">";
  content2 += "<div data-role=\"header\">";
  content2 += "<h1>NanodeUIP</h1>";
  content2 += "</div>";
  content2 += "<div data-role=\"content\">";
  content2 += "<ul data-role=\"listview\" data-theme=\"g\">";
  content2 += "<li><a href=\"/buttons.shtml\">Buttons</a></li>";
  content2 += "<li><a href=\"/lights.shtml\">Lights</a></li>";
  content2 += "<li><a href=\"/sensors.shtml\">Sensors</a></li>";
  content2 += "</ul>";
  content2 += "</div>";
  content2 += "<div data-role=\"footer\" class=\"ui-bar\">";
  content2 += "<a href=\"/credits.shtml\" data-icon=\"info\">Credits</a>";
  content2 += "<a href=\"http://maniacbug.wordpress.com/2012/01/28/webserver/\" data-icon=\"star\">Help</a>";
  content2 += "</div>";
  content2 += "</div>";
  content2 += "</body>";
  content2 += "</html>";

  header += "Content-Length:";
  header += (int)(content.length()+content2.length());
  header += "\r\n\r\n";

  esp.print("AT+CIPSEND=");
  esp.print(ch_id);
  esp.print(",");
  esp.println(header.length()+content.length()+content2.length());
  if (waitforit (1000))  // we wait for the "> "
  {
    esp.print(header);
    esp.print(content);
    esp.print(content2);
  }
}

void serve_homepage_ian(int ch_id) //this serves the page
{
  String Header;

  Header =  "HTTP/1.1 200 OK\r\n";            //bog standard stuff - should provide alternative headers
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
//  Header += "Refresh: 1\r\n";

  String Content;
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> ESP8266 test </title>";
  Content += "<H1>";
  Content += "ESP8266 Web Server Test Page, Reloads = ";
  Content += String(serverreq_num);
  Content += "<br>";
  Content += "This is channel ";
  Content += String(ch_id);
  Content += " of 0-4 available channels";
  Content += "</H1>";
  Content += "<H2>";
  Content += "BMP180 pressure & temperature transducer with Adafruit library\n<br><br>";
  Content += "Pressure = ";
  Content += String(-1);
  Content += " Pa";
  Content += "<br>";
  Content += "Temperature = ";
  Content += String(-1);
  Content += " *C";
  Content += "<br>";
  //Content += "Altitude = ";
  //Content += String(bmp.readAltitude());
  //Content += " metres";
  Content += "<br><br><br>";
  Content += "<i><a href=\"mailto:xsexton@dmu.ac.uk?subject=ESP8266%20Webserver\">Ian Sexton 2014</a></i><br>";
  Content += "</H2>";

  Header += "Content-Length: ";  //ESP likes to know the length
  Header += (int)(Content.length());  //length determined here
  Header += "\r\n\r\n";    //blank line

  Serial1.print("AT+CIPSEND=");    //send the web page
  Serial1.print(ch_id);
  Serial1.print(",");
  Serial1.println(Header.length()+Content.length());
  if (waitforit (1000))  // we wait for the "> "
  {
    Serial1.print(Header);
    Serial1.print(Content);
  }
}

void serve_stats(int ch_id) //this serves the page
{
  String Header;

  Header =  "HTTP/1.1 200 OK\r\n";            //bog standard stuff - should provide alternative headers
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
//  Header += "Refresh: 1\r\n";

  String Content;
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> ESP8266 test </title>";
  Content += "<H1>";
  Content += "ESP8266 Web Server Test Page, loads = ";
  Content += String(serverreq_num);
  Content += "<br>";
  Content += "This is channel ";
  Content += String(ch_id);
  Content += " of 5 available channels";
  Content += "<br>";
  Content += "ESP8266 restarts: ";
  Content += String(esp_rst_num);
  Content += "</H1>";
  Content += "<br><br><br>";
  Content += "<H2>";
  Content += "<i><a href=\"mailto:anybody@anywhere.com?subject=ESP8266%20Webserver\">Someone</a></i><br>";
  Content += "</H2>";

  Header += "Content-Length: ";  //ESP likes to know the length
  Header += (int)(Content.length());  //length determined here
  Header += "\r\n\r\n";    //blank line

  Serial1.print("AT+CIPSEND=");    //send the web page
  Serial1.print(ch_id);
  Serial1.print(",");
  Serial1.println(Header.length()+Content.length());
  if (waitforit (1000))  // we wait for the "> "
  {
    Serial1.print(Header);
    Serial1.print(Content);
  }
}

