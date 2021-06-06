# wejoy v0.2

'wejoy' is a tool to read physical joystick devices, aswell as keyboards, create virtual joystick devices and output keyboard presses on a Linux system. It uses helpful sourcecode which in somewhat degree is modified:
* https://github.com/tuomasjjrasanen/libsuinput
* https://github.com/drewnoakes/joystick
* https://github.com/EliasD/unnamed_lua_binder/


# Features ----
* Create one or more virtual joysticks with a custom number of buttons and axes.
* Read events from several connected keyboards
* Read physical joystick buttons and and axes events.
* Send keyboard events
* Lua scripting for conditional purposes


# Known requirements ----
* liblua version 5.2 or later.
* libudev

Development files for liblua and libudev are required for compliation:

1. Search for which version of liblua you may have in your version of Linux distribution:

$ apt-cache search liblua

You may have liblua5.2 or a newer version.

Then install the developments files of you version of liblua and libudev i.e:
* $ sudo apt-get install liblua5.x-dev libudev-dev

Also install the shared libraries of liblua and libudev i.e:
* $ sudo apt-get install liblua5.x-0 libudev1

Where 5.x is the available liblua version of your distro.


# Compile ----
* $ sh make.sh
* This will create an executable called 'wejoy' which is run from the terminal.
* NOTE: If another version than liblua 5.2 is installed, the make.sh needs to be edited for the current version installed.


# Usage ----
If the module uinput is not loaded on your system, you need to manually load it:
* $ sudo modprobe uinput
*
* As for now, 'wejoy' need to be run as root:
* $ sudo ./wejoy script.lua
* Where script.lua is your preferred configuration file.
*
* You can quit 'wejoy' by pressing 'q' end then 'ENTER'.

# Install it as a service ----

If you want to have a permanent config script running, you may also install wejoy as a service by running the `installService.sh` script
This script will create a systemd service that starts on boot and copy the script config you want to `/etc/wejoy/config.lua`.
Example: `sudo ./installService.sh il2_missing_axis.lua`

# Run it with a steam game

If you only want to run it with a steam game, you can set the game's launch options as: `/route/to/wejoy-run.sh %command%`. This way wejoy will launch automatically when you launch that game. You can also clone the SH file and customize a lua config script for each game.

# LUA scripting ----
* Please read the example.lua and warthog_throttle.lua to learn how to customize your script.
* Also read the keycodes_ref.txt for keyboard reference. These variables are globally accessable in your LUA script.


# KNOWN BUGS ----
* Every axis and button is not tested to work fully.
