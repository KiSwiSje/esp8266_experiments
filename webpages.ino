// global strings in progmem
const char responder[] PROGMEM =  "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: %d\r\n\r\n";  // vb 200 OK contentlength
// 200 OK 403 Forbidden 404 Not Found http://www.w3.org/Protocols/rfc2616/rfc2616.html

// bigredbutton
const char brb1[] PROGMEM =       "<!DOCTYPE html><html><title>Big Red Button</title><body><h1>Push the button</h1>";
const char brb2[] PROGMEM =       "<img src=\"http://upload.wikimedia.org/wikipedia/commons/2/29/Red_button.png\"";
const char brb3[] PROGMEM =       "alt=\"Big Red Button\" usemap=\"#brb\" style=\"width:300px;height:300px;border:0\">";
const char brb4[] PROGMEM =       "<map name=\"brb\"><area shape=\"circle\" coords=\"147,123,120\" href=\"b.html\"></map></body></html>";
//                                 1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890 <-- 100 chars
// not found
const char not1[] PROGMEM =       "<html><head><title>Page Not Found</title></head><body><h3>";
const char not2[] PROGMEM =       "The page could not be found</h3><p><em>Use the browser's back button to go back<br />";
const char not3[] PROGMEM =       "Don't bother reporting the issue, nobody cares</em></p></body></html>\r\n";

// show bigredbutton
void webpage_brb(int ch_id)
{
  char asciibuf[110];
  int contentlength = strlen_P (brb1)+strlen_P (brb2)+strlen_P (brb3)+strlen_P (brb4);

ser.print (F("******")); ser.println (contentlength);        // 325
ser.print (F("******")); ser.println (strlen_P(responder));  // 82
ser.print (F("******")); ser.println (freeRam());            // 3577 (from 3587)

  sprintf_P (asciibuf, responder,200,"OK",contentlength);

  esp.print(F("AT+CIPSEND="));    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(strlen(asciibuf)+contentlength);
  if (waitforit (1000))  // we wait for the "> "
  {
    esp.print(asciibuf);
    sprintf_P (asciibuf, brb1);
    esp.print(asciibuf);
    sprintf_P (asciibuf, brb2);
    esp.print(asciibuf);
    sprintf_P (asciibuf, brb3);
    esp.print(asciibuf);
    sprintf_P (asciibuf, brb4);
    esp.print(asciibuf);
  }
}

void webpage_notfound(int ch_id)
{
  char asciibuf[110];
  int contentlength = strlen_P (not1)+strlen_P (not2)+strlen_P (not3);

ser.print (F("******")); ser.println (contentlength);        // 
ser.print (F("******")); ser.println (strlen_P(responder));  // 
ser.print (F("******")); ser.println (freeRam());            // 

  sprintf_P (asciibuf, responder,404, "Not Found",contentlength);

  esp.print(F("AT+CIPSEND="));    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(strlen(asciibuf)+contentlength);
  if (waitforit (1000))  // we wait for the "> "
  {
    esp.print(asciibuf);
    sprintf_P (asciibuf, not1);
    esp.print(asciibuf);
    sprintf_P (asciibuf, not2);
    esp.print(asciibuf);
    sprintf_P (asciibuf, not3);
    esp.print(asciibuf);
  }
}

// show statistics - needs work - move strings to progmem
void serve_homepage_stats(int ch_id)
{
  char buf[20];
  String Header;

  Header =  "HTTP/1.1 200 OK\r\n";              
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  Header += "Refresh: 10\r\n";                  // note - refreshes every 10s

  String Content;
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> ESP8266 test </title>";
  Content += "<H1>";
  Content += "esp starts ";
  Content += String(esp_restart_num);
  Content += "<br>";
  Content += "ESP8266 Web Server Test Page, loads = ";
  Content += String(serverreq_num);
  Content += "<br>";
  Content += "This is channel ";
  Content += String(ch_id);
  Content += " of 5 available channels";
  Content += "<br>";
  Content += "ESP8266 start ok: ";
  Content += String(server_startok_num);
  Content += "<br>";
  Content += "ESP8266 start nok: ";
  Content += String(server_startnok_num);
  Content += "<br>";
  Content += "free ram: ";
  Content += String(freeRam());
  Content += "<br>";
  Content += "Uptime: ";
  Content += readabletime_string (millis(), buf);
  Content += "</H1>";
  Content += "<br><br><br>";
  Content += "<H2>";
  Content += "<i><a href=\"1.html\">go to page ray</a></i><br>";
  Content += "<i><a href=\"2.html\">go to page nanode</a></i><br>";
  Content += "<i><a href=\"3.html\">go to page Ian</a></i><br>";
  Content += "<i><a href=\"4.html\">go to page notavail</a></i><br>";
  Content += "<i><a href=\"mailto:anybody@anywhere.com?subject=ESP8266%20Webserver\">Someone</a></i><br>";
  Content += "</H2>";

  Header += "Content-Length: ";  //ESP likes to know the length
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
