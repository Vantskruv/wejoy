# wejoy v0.1

'wejoy' is a tool to read physical joystick devices, create virtual joystick devices and output keyboard presses on a Linux system. It uses helpful sourcecode which in somewhat degree is modified:
* https://github.com/tuomasjjrasanen/libsuinput
* https://github.com/drewnoakes/joystick
* https://github.com/EliasD/unnamed_lua_binder/


# Features ----
* Create one or more virtual joysticks with a custom number of buttons and axes.
* Read physical joystick buttons and and axes events.
* Send keyboard events
* Lua scripting for conditional purposes


# Known requirements ----
liblua5.2


# Compile ----
* $ sh make.sh
* This will create an executable called 'wejoy' which is run from the terminal.


# Usage ----
If the module uinput is not loaded on your system, you need to manually load it:
* $ sudo modprobe uinput
*
* As for now, 'wejoy' need to be run as root:
* $ sudo ./wejoy script.lua
* Where script.lua is your preferred configuration file.
*
* You can quit 'wejoy' by pressing 'q' end then 'ENTER'.

# LUA scripting ----
* Please read the example.lua and warthog_throttle.lua to learn how to customize your script.
* Also read the keycodes_ref.txt for keyboard reference. These variables are globally accessable in your LUA script.


# KNOWN BUGS ----
* An output of error loading last not set index of virtual device and physical device is shown, which should not be an error. This is however not fatal.
* Every axis and button is not tested to work fully.
