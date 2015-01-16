/*

void serve_homepage_ray(int ch_id)
{
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n";

  String content="";
  // output the value of each analog input pin
  for (int analogChannel = 0; analogChannel < 6; analogChannel++)
  {
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

// just a quick test to show device independant page/external scripting
// for more info check out maniacbugs site
void serve_homepage_nanode(int ch_id)
{
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n";

  String content = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">";

  content += "<html>";
  content += "<head>";
  content += "<title>needs work!!</title>";
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

  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
//  Header += "Refresh: 1\r\n";

  String Content;
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> esp8266 test </title>";
  Content += "<H1>";
  Content += "esp8266 Web Server Test Page, Reloads = ";
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
  Content += "<br><br><br>";
  Content += "<i><a href=\"mailto:xsexton@dmu.ac.uk?subject=esp8266%20Webserver\">Ian Sexton 2014</a></i><br>";
  Content += "</H2>";

  Header += "Content-Length: ";  //esp likes to know the length
  Header += (int)(Content.length());  //length determined here
  Header += "\r\n\r\n";    //blank line

  esp.print("AT+CIPSEND=");    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(Header.length()+Content.length());
  if (waitforit (1000))  // we wait for the "> "
  {
    esp.print(Header);
    esp.print(Content);
  }
}

// show statistics
void serve_homepage_stats(int ch_id)
{
  String Header;

  Header =  "HTTP/1.1 200 OK\r\n";              
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  Header += "Refresh: 10\r\n";                  // note - refreshes every 10s

  String Content;
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> esp8266 test </title>";
  Content += "<H1>";
  Content += "esp8266 Web Server Test Page, loads = ";
  Content += String(serverreq_num);
  Content += "<br>";
  Content += "This is channel ";
  Content += String(ch_id);
  Content += " of 5 available channels";
  Content += "<br>";
  Content += "esp8266 restarts: ";
  Content += String(esp_rst_num);
  Content += "<br>";
  Content += "free ram: ";
  Content += String(freeRam());
  Content += "</H1>";
  Content += "<br><br><br>";
  Content += "<H2>";
  Content += "<i><a href=\"1.html\">go to page ray</a></i><br>";
  Content += "<i><a href=\"2.html\">go to page nanode</a></i><br>";
  Content += "<i><a href=\"3.html\">go to page Ian</a></i><br>";
  Content += "<i><a href=\"4.html\">go to page notavail</a></i><br>";
  Content += "<i><a href=\"mailto:anybody@anywhere.com?subject=esp8266%20Webserver\">Someone</a></i><br>";
  Content += "</H2>";

  Header += "Content-Length: ";  //esp likes to know the length
  Header += (int)(Content.length());  //length determined here
  Header += "\r\n\r\n";    //blank line

  esp.print("AT+CIPSEND=");    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(Header.length()+Content.length());
  if (waitforit (1000))  // we wait for the "> "
  {
    esp.print(Header);
    esp.print(Content);
  }
}

*/
