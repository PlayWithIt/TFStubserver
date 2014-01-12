TFStubserver
============

A software emulator for Tinkerforge Hardware

This software emulates some of the Tinkerforge hardware (see
http://www.tinkerforge.com) on network protocol level. A client
can connect as if it would connect to 'brickd'.


See also:
http://www.tinkerunity.org/wiki/index.php/DE/Projekte/Stackemulator_%28stubserver%29


Requirements:
-------------
* Linux
* g++ 4.6.3 or higher (as on Raspberry PI, OpenSuse 13.1)
* clang++ 3.3 (llvm) or higher should work too
* ALSA devel packages (libasound2-dev on Debian, alsa-devel on OpenSuse)


Compile:
--------
* run 'make' in the base directory for a debug-build
* or run 'make CONFIG=Release' for an optimized build
* or run 'make USE_LLVM=1' for a build with clang++

Example:
--------
* example configurations are in 'src/stubserver/devices.properties' and
'src/stubserver/homestation.properties'
* see 'stubserver -?' for command-line options
* 'src/stubserver/stub' is an example to start the server on port 4225
