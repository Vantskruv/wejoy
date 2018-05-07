cmake_minimum_required(VERSION 3.6)
project(wejoy)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cc suinput.c joystick.cc LuaScript.cc CVirtualJoy.cc CVirtualKeyboard.cc)
add_executable(wejoy ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(wejoy udev pthread X11 lua5.2 evdev)