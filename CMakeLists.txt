cmake_minimum_required(VERSION 3.6)   # CMake version check
project(wejoy)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 11)            # Enable c++11 standard

set(SOURCE_FILES main.cc suinput.c joystick.cc LuaScript.cc CVirtualJoy.cc CVirtualKeyboard.cc)            # Add main.cpp file of project root directory as source file
add_executable(wejoy ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(wejoy udev pthread X11 lua5.2 evdev)