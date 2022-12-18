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

Requirements:
-------------
* Linux (also compiles on Tinkerforge's RED Brick or RasPi)
* g++ 11 or higher (C++ 17)
* clang++ 9 (llvm) or higher should work too
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
  (e.g. https://github.com/PlayWithIt/TFStubserver/blob/master/src/stubserver/motion.properties). The properties are used for device
  configuration
* Sensors mostly use a 'ValueProvider' which calculates the values or reads
  the value from a text file (when when value changes + value).
* see ``stubserver -?`` for command-line options
* ``src/stubserver/stub`` is an example to start the server on port 4225
* your application or brickv can then connect to the emulated devices via
  ``localhost:4225``

Configuration properties:
-------------------------
* a property file must have a setting ``UIDS`` which holds the space separated list of brick/bricklet UIDS which should be emulated.
* the setting ``UI_UIDS`` is the for UI only (see TFStubViz above): this is a list of bricks/bricklets shown in the Qt UI. This is a subset of ``UIDS`` and you can for example exclude the master brick(s) to have a reduced set, only the relevant bricklets in the UI. 
* for each UID out of that list further settings are required
  * ``<UID>.type``: a device type which is derived from the DEVICE_IDENTIFIED constant out of the bindings
  * ``<UID>.firmwareVersion``: firmware version to report, 202 mean 2.0.2; 2410 means 2.4.10
  * ``<UID>.hardwareVersion``: hardware version to report. The emulator does nothing with the version, but a client might use that. These versions are reported to the client via enumeration.
  * ``<UID>.connectedTo``: empty for a brick which is not connected to another one, otherwise this is the UID of the brick where this brick or bricklet is connected to
  * ``<UID>.position``: position in the stack for bricks (0..9) or port number where a bricklet is connected to a brick (a..i). This needs to be a proper combination.
  * ``<UID>.valueProvider``: optional depending on the kind of device, most sensors need a value provider which generates the sensor values. Some devces need more than one valueProvider as they have more than one sensor channel.
  
Value providers:
----------------
Possible settings for the ``valueProvider`` property:
* ``const value=<xx>``: generates a constant value that never changes
* ``onOff interval=<msI>,duration=<msD>``: generates 0 / 1 values where the value is 1 for ``msD`` milliseconds out of a cycle of  ``msI`` milliseconds. ``msI`` must be larger than ``msD``
* ``linear min=<min>,max=<max>,step=<step>,interval=<msI>``: the value starts with ``min`` and is changed every ``msI`` milliseconds by adding ``step``. If the actaul value if below min or above max, then the step gets negated. This creates a kind of zig-zag pattern.
* ``sinus min=<min>,max=<max>,interval=<msI>``: creates a sinus curve with a duration of ``msI`` milliseconds and mentioned min/max values.
* ``random min=<min>,max=<max>,interval=<msI>``: generates random numbers which changes every ``msI`` milliseconds, each random value is in the range min..max.
* ``stored <filename>``: read time when to change and sesnor value from a file.
* ``csv <filename>``: one line represents one set of values. The first column is the time offset in milliseconds, the other columns are data values.