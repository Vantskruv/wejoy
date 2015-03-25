#!bin/sh
#g++ -ludev suinput.o -o test main.cc -Wall
g++ -Wall -std=c++11 -ludev -lpthread -lX11 -llua5.2 suinput.c joystick.cc LuaScript.cc CVirtualJoy.cc CVirtualKeyboard.cc -o wejoy main.cc
#g++ test.cc joystick.cc -std=c++0x -Wall -o test
