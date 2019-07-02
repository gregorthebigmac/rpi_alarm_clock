
# Important Details Follow!

Okay, so this program will seem a bit complicated and weird, so let me break it down as simply as I can. The main program (i.e. starting point) for this whole thing is in alarm.cpp. From there, if you're experienced in C/C++ you should be able to find your way around the code fairly easily. For the rest of you, there are a decent number of dependencies, and I included installation instruction links, but I won't be detailing how to install them here--I can barely remember how I installed half of these libs on my own machine, tbh. That said, if you're C/C++ savvy, here's the gist of what you need to know.

rpi_serial.c is the only source file that needs to be compiled by itself and made into its own *separate* binary. Compile it with the following command:

    $ gcc -Wall -pthread -o rpi_serial rpi_serial.c -lpigpio -lrt

Everything else should compile into a singular binary, and you'll do that with the following command:

    $ g++ `sdl-config --cflags --libs` alarm.cpp -lSDL_mixer -o alarm

***Note:*** If you want to call the program something else, that's fine, just make sure you know what you're doing. If you change the name of the binary for rpi_serial, you'll have to change where it's called in alarm.cpp before compiling alarm.cpp, otherwise it will call the program, it won't be found, and you're gonna have a bad time. If you change the binary name for the alarm, that won't change anything.

***2nd Note:*** The audio file should be of the .ogg type, and should be placed in the /data directory, and named "alarm.ogg" (or you can change the filename in the source if you like).

## Dependencies

This program depends on the **pigpio library,** found here: <http://abyz.me.uk/rpi/pigpio/index.html>

This library is how the RPi talks to the Arduino (and vice/versa) via the serial TX<->RX pins. That's all it does. Is it overkill? Probably. But it was the easiest method I could find (that involves C/C++) to communicate via serial. If you have a better way, feel free to do it in Python or whatever, and just change the program call in alarm.cpp from [rpi_serial.c] to [whatever.py]

This program also depends on the **SDL mixer 1.2 library.** I'll get a link to the download for the lib when I get some time to go back and confirm where I found it for the RPi. Just make sure you get the **DEVELOPER** library, not the regular one!

**UPDATE:** I don't think you *need* the developer library to make that work, but if getting the regular version gives you any problems, try using the developer version and see if that fixes it. I'll gladly accept any feedback regarding this (or anything else in this project, for that matter).

This is how the alarm plays music/sfx/etc. You can use whichever audio output you want (it should have no problems working with either the 3.5mm jack or the HDMI. However, I only tested this with the 3.5mm jack, so be advised).

If you have any questions, feedback, concerns, bithces, gripes, complaints, etc. feel free to leave comments here, and I'll see what I can do. Also, if you wish to contribute to the code, by all means, do so! Send a merge request if you got something that works, and as long as it works, I'll either approve it, or give it its own branch!
