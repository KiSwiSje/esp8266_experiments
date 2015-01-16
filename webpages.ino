// global strings in progmem
const char responder[] PROGMEM =  "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: %d\r\n\r\n";  // vb 200 OK contentlength
// 200 OK 403 Forbidden 404 Not Found http://www.w3.org/Protocols/rfc2616/rfc2616.html

// bigredbutton
const char brb1[] PROGMEM =       "<!DOCTYPE html><html><title>Big Red Button</title><body><h1>Push the button</h1>";
const char brb2[] PROGMEM =       "<img src=\"http://upload.wikimedia.org/wikipedia/commons/2/29/Red_button.png\"";
const char brb3[] PROGMEM =       "alt=\"Big Red Button\" usemap=\"#brb\" style=\"width:300px;height:300px;border:0\">";
const char brb4[] PROGMEM =       "<map name=\"brb\"><area shape=\"circle\" coords=\"147,123,120\" href=\"_.html\"></map></body></html>";
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

//ser.print (F("******")); ser.println (contentlength);        // 325
//ser.print (F("******")); ser.println (strlen_P(responder));  // 82
//ser.print (F("******")); ser.println (freeRam());            // 3577 (from 3587)

  sprintf_P (asciibuf, responder,200,"OK",contentlength);
  ser.print (F("******header_len")); ser.println (strlen(asciibuf));

  esp.print(F("AT+CIPSEND="));    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(strlen(asciibuf)+contentlength);
  if (waitforit (1000,""))  // we wait for the "> "
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
  ser.print (F("\n******content_len")); ser.println (contentlength);
  ser.print (F("******freeram")); ser.println (freeRam());
  ser.print (F("******requester")); ser.println (requester);
  ser.print (F("******tictoc")); ser.println (millis ()-tictoc);
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
  if (waitforit (1000,""))  // we wait for the "> "
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
  Content += "<title> esp8266 statistics </title>";
  Content += "<H1>";
  Content += "Uptime: ";
  Content += readabletime_string (millis(), buf);
  Content += "<br>";
  Content += "esp restarts: ";
  Content += String(esp_restart_num);
  Content += "<br>";
  Content += "last restart: ";
  Content += readabletime_string (millis()-last_esp_restart, buf);
  Content += " ago";
  Content += "<br>";
  Content += "pages served: ";
  Content += String(serverreq_num);
  Content += "<br>";
  Content += "This is channel ";
  Content += String(ch_id);
  Content += " of 5 available channels (0-4)";
  Content += "<br>";
  Content += "server_ch_id_max: ";
  Content += String(server_ch_id_max);
  Content += "<br>";
  Content += "esp8266 start ok: ";
  Content += String(server_startok_num);
  Content += "<br>";
  Content += "esp8266 start nok: ";
  Content += String(server_startnok_num);
  Content += "<br>";
  Content += "esp8266 connection unresponsive: ";
  Content += String(server_unresponsive_num);
  Content += "<br>";
  Content += "chunksize: ";
  Content += String(chunksize);
  Content += "<br>";
  Content += "free ram: ";
  Content += String(freeRam());
  Content += "</H1>";
  Content += "<br><br><br>";
  Content += "<H2>";
  Content += "<i><a href=\"/\">go to page brb</a></i><br>";
  Content += "<i><a href=\"x.html\">go to page notavail</a></i><br>";
  Content += "<i><a href=\"mailto:anybody@anywhere.com?subject=esp8266%20Webserver\">Someone</a></i><br>";
  Content += "</H2>";

  Header += "Content-Length: ";  //esp likes to know the length
  Header += (int)(Content.length());  //length determined here
  Header += "\r\n\r\n";    //blank line

  esp.print("AT+CIPSEND=");    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(Header.length()+Content.length());
  if (waitforit (1000,""))  // we wait for the "> "
  {
    esp.print(Header);
    esp.print(Content);
  }
  ser.print (F("\n******content_len")); ser.println (Content.length());
  ser.print (F("******header_len")); ser.println (Header.length());
  ser.print (F("******freeram")); ser.println (freeRam());
  ser.print (F("******requester")); ser.println (requester);
  ser.print (F("******tictoc")); ser.println (millis ()-tictoc);
}

// test buttons
void serve_homepage_boven(int ch_id)
{
  char buf[20];
  String Header;
  byte trying=0;
  int i=0;
  int j;
  int l;
  byte ok;

//           123456789012345 6 7
  Header =  "HTTP/1.1 200 OK\r\n";         
//           89012345678901234567890 1 2  
  Header += "Content-Type: text/html\r\n";
//           34567890123456789 0 1 =61
  Header += "Connection: close\r\n";  
//  Header += "Refresh: 10\r\n";                  // note - refreshes every 10s

  String Content;
  Content = "<!DOCTYPE html><html><head><style>";
  Content += "body {background-color: palegreen;font-family:verdana;}";
  Content += "h1 {text-align: center;}";
  Content += "a {font-size:300%;display: inline;font-weight: bold;color: white;width: 140px;";
  Content += "text-align: center;padding: 10px;text-decoration: none;}";
  Content += "a.on:link, a.on:visited {background-color: darkgreen;}";
  Content += "a.on:hover, a.on:active {background-color: red;}";
  Content += "a.of:link, a.of:visited {background-color: red;}";
  Content += "a.of:hover, a.of:active {background-color: darkgreen;}";
  Content += "a.xx:link, a.xx:visited {visibility: hidden;}";
  Content += "</style></head><body><title>Boven</title><h1>Boven</h1>";

  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"of\" href=\"1.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"of\" href=\"5.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"of\" href=\"2.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<br><br><br>";

  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"A.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"B.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"C.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<br><br><br>";

  Content += "<a class=\"on\" href=\"D.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"E.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"F.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"G.html\">O</a>";
  Content += "<br><br><br>";

  Content += "<a class=\"on\" href=\"H.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"I.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"J.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"K.html\">O</a>";
  Content += "<br><br><br>";

  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"L.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"M.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"N.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<br><br><br>";

  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"3.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"6.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";
  Content += "<a class=\"on\" href=\"4.html\">O</a>";
  Content += "<a class=\"xx\" href=\"x.html\">O</a>";

  Content += "</body></html>";
// 61 + 
//           2345678901234567
  l=Content.length();  // debugging - stond in de while loop

  Header += "Content-Length: ";  //esp likes to know the length
  //        89
  Header += (int)(l);  //length determined here
//            0 1 2 3
  Header += "\r\n\r\n";    //blank line

  esp.print("AT+CIPSEND=");    //send the web page
  esp.print(ch_id);
  esp.print(",");
  esp.println(Header.length());    // send header first

  ok = waitforit (5000,"");  // we wait for the "> "
  if (ok)
  {
    esp.print(Header);

    ok = waitforit (5000,"SEND OK\r\n");  // did header go through?
  }
  if (!ok)
    ser.print (F("\nsomething went wrong sending the header\n"));
  if (ok)
  {
    while (l > 0)
    {
      esp.print("AT+CIPSEND=");    // cipsend command
      esp.print(ch_id);
      esp.print(",");
      if (l > chunksize)
      {
        esp.print(chunksize);
        j=chunksize;
      }
      else
      {
        esp.print(l);
        j=l;
      }
      esp.print (F("\r\n"));
  
      ok = waitforit (1000,"");  // wait for >
      if (ok)
      {
        for (; j>0; j--)              //send a chunk
          esp.print(Content[i++]);        // ************* debugging **************
        l-=chunksize;
        ok = waitforit (5000,"SEND OK\r\n");
      }
      if ( (!ok) || (serverup==0) )
      {
        l=0;  // exit the while loop - something went wrong
        ser.print (F("\nsending chunk-something went wrong ")); ser.print (trying); ser.println ();
      }
    }
  }

  ser.print (F("\n******content_len")); ser.println (Content.length());
  ser.print (F("******header_len")); ser.println (Header.length());
  ser.print (F("******freeram")); ser.println (freeRam());
  ser.print (F("******requester")); ser.println (requester);
  ser.print (F("******tictoc")); ser.println (millis ()-tictoc);
}

