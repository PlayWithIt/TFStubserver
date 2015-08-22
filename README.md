TFStubserver
============

A software emulator for Tinkerforge Hardware

This software emulates some of the Tinkerforge hardware (see
http://www.tinkerforge.com) on network protocol level. A client
can connect as if it would connect to 'brickd'.


The emulator fully supports callbacks and configurable sensor values. The 
configurable sensor values are in some kind "predictable", which is useful
for unit testing.
In combination with a Qt based UI (see https://github.com/PlayWithIt/TFStubViz),
you can manually override/control the sensor values using sliders or buttons
(depending on the type). Doing this you can for example test some extreme or
very specific combinations.


See this page to get a list of supported Tinkerforge devices and a short introduction:
http://www.tinkerunity.org/wiki/index.php/DE/Projekte/Stackemulator_%28stubserver%29

Requirements:
-------------
* Linux (also compiles on Tinkerforge's RED Brick or RasPi)
* g++ 4.6.3 or higher (as on Raspberry PI, OpenSuse 13.1)
* clang++ 3.3 (llvm) or higher should work too
* optional: ALSA devel packages (libasound2-dev on Debian, alsa-devel on OpenSuse)

Compile:
--------
* run ``make`` or ``make -j4`` in the base directory for a debug-build
* or run ``make CONFIG=Release`` for an optimized build
* or run ``make USE_LLVM=1`` for a build with clang++
* or run ``make DONT_USE_ALSA=1`` to exclude ALSA support if ALSA is not installed

Example:
--------
* example configurations are in ``src/stubserver``, look for ``*.properties``
  (e.g. ``motion.properties``). The properties are used for device
  configuration
* Sensors mostly use a 'ValueProvider' which calculates the values or reads
  the value from a text file (when when value changes + value).
* see ``stubserver -?`` for command-line options
* ``src/stubserver/stub`` is an example to start the server on port 4225
* your application or brickv can then connect to the emulated devices via
  ``localhost:4225``
