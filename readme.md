#WeJoy

#How to clone
git clone git@gitlab.ecs.vuw.ac.nz:swen301-2018/govindsanj/SWEN301-Project-2.git

#How to build
cmake .

#How to run
sudo ./wejoy path-to-script

#Requirements
A linux computer, with lua5.2, uinput, libevdev, and sudo privileges
Note that if you would like to test the application with real hardware, 
a game controller of some description is required, but the tests virtualize
one so this is not required.

To test controller output manually, something like sdl2-jstest is required.

#How do scripts work
Check an example script such as `scripts/wiimote_guitar.lua` for new features, 
and `scripts/example.lua` for the original syntax.

