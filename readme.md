esp8266_experiments
===================

esp8266 wifi module - and some simple ways to talk to it

there seem to be many different versions of the firmware around, and each reacts differently to the commands sent to it

I created this sketch to be able to try individual commands - and pieces of code - and see how the esp8266 reacts.

the idea is simple - what you type in the serial monitor gets sent to the serial port the esp8266 is connected to
the output from the esp8266 is shown back in the serial monitor, but for added clarity the control characters are displayed in c-format (meaning newline is shown as \n, carriage return as \r, etc)
this allows to study what sort of responses you can get after a command - and how to deal with it in your code

i have also incorporated a way to check if the esp8266 restarts - and if need be, restart your server that was running.
this may not work on all versions of the firmware - i think later versions have a watchdog timer that detects a hang-up and restarts the esp8266

I also included some code I found, that runs a basic http server to show some data (analog ports)
this code is from:  Ray Wang @ Rayshobby LLC http://rayshobby.net/?p=9734
I adapted it to fit into my programs structure


as of writing - i am using:
esp8266 firmware 0018000902-AI03 (on independant 3.3v)
arduino mega2560 on usb - so serial goes to the computer, serial1 goes to the esp8266
level shifter CD4050 to protect the esp8266

aparantely my computer is too old - I can't run git on the commandline, nor the gui program. And even the browsers that my system can handle are too old :(
I will try and work around this

good news ... Jamie from Github staff replied to me whining that a mac from 2007 should be able to run git, and pointed me to an old deprecated version that my piece of antique hardware can run. so please bear with me if i can't access all of the features.

