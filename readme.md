# WeJoy

# How to clone
git clone git@gitlab.ecs.vuw.ac.nz:swen301-2018/govindsanj/SWEN301-Project-2.git

# How to build
cmake .

# How to run
sudo ./wejoy path-to-script

# Original Code:
the original_code folder contains the original code, modified to disable the hard requirement on
USB devices. the tests have also been copied over, as the original code required much longer timings
so the tests were incompatible.

# Requirements
A linux computer, with lua5.2, uinput, libevdev, and sudo privileges
Note that if you would like to test the application with real hardware, 
a game controller of some description is required, but the tests virtualize
one so this is not required.

To test controller output manually, something like sdl2-jstest is required.

# How do scripts work
Check an example script such as `scripts/wiimote_guitar.lua` for new features, 
and `scripts/example.lua` for the original syntax.

# Changes
- Removed usb limitation, supports any type of device
- /dev/input/js* -> /dev/input/event*
- testing framework
- functions for manipulating many axis/buttons at once
- ability to get min and max values
- functions based on the names of axis/buttons when available
- importer written in python that parses the linux headers with axes/buttons, and exposes this to lua

# Testing
The testing framework uses `Catch2`, and it works by creating a virtual joystick, then manipulating 
it and checking that output from the wejoy virtual joysticks / keyboard make sense.

# Testing the original code
A modified version of the original code is provided that has no architectural changes, but instead 
has modified udev code so it does not have a reliance on USB anymore. This means all the new tests can run
- however, it appears the original code has issues with checking if a button is down after pushing it,
and the timings have all been changed as the original code base was slower to update.