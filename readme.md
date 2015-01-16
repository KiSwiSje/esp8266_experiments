esp8266_experiments
===================

esp8266 wifi module - and some simple ways to talk to it
note: this device is far from reliable - expect frequent restarts - even hanging connections - handling these automatically is a bit of a challenge

I must admit - i had a bit of a struggle - i somehow lost reliability of the server (esp8266 restarts a lot!)
but i think i managed to solve some problems
the code is a huge mess, though - will try and clean it up

there seem to be many different versions of the firmware around, and each reacts differently to the commands sent to it

I created this sketch to be able to try individual commands - and pieces of code - and see how the esp8266 reacts.

the idea is simple - what you type in the serial monitor gets sent to the serial port the esp8266 is connected to
the output from the esp8266 is shown back in the serial monitor, but for added clarity the control characters are displayed in c-format (meaning newline is shown as \n, carriage return as \r, etc)
this allows to study what sort of responses you can get after a command - and how to deal with it in your code

i have also incorporated a way to check if the esp8266 restarts - and if need be, restart your server that was running.
this may not work on all versions of the firmware - i think later versions have a watchdog timer that detects a hang-up and restarts the esp8266 (confirmed - can be enabled/disabled - does not seem to change the frequent restarts though - will investigate further)

I adapted a simple webserver example, I included a couple of simple web pages for testing purposes.
(sources can be found in the code)

as of writing - i am using:
esp8266 firmware 0018000902-AI03 (on independant 3.3v)
arduino mega2560 on usb - so serial goes to the computer, serial1 goes to the esp8266
level shifter CD4050 to protect the esp8266
update: my sketch handles restarts from the esp8266 well, so far i did not have to reset it. The browser may not load, or stop loading the auto-refreshing page though.
I did notice that with a baudrate of 9600 the esp8266 restarts a lot when i try and connect from 2 browsers. (was very stable as long as only one browser connected) you can actually change this baudrate with AT+CIOBAUD=57600. this seems to help a lot.
I am doing some more testing - I will update









aparantely my computer is too old - I can't run git on the commandline, nor the gui program. And even the browsers that my system can handle are too old :(
I will try and work around this

good news ... Jamie from Github staff replied to me whining that a mac from 2007 should be able to run git, and pointed me to an old deprecated version that my piece of antique hardware can run. so please bear with me if i can't access all of the features of github.

