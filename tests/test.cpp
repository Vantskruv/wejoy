#include <iostream>
#include "controller.h"
//
// Created by sanjay on 5/8/18.
//
int main(int argc, char **argv) {
    std::cout << "starting vcontroller" << std::endl;
    Controller vjoy(3,2,0x044f,0x0402,"Thrustmaster Warthog Joystick");
    Controller vjoy2(3,2,0x044f,0x0404,"Thrustmaster Warthog Joystick2");
    std::cout << "starting app" << std::endl;
    system("../wejoy ../../scripts/example.lua");
    return 0;
}
