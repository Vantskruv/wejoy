#include "controller.h"
//
// Created by sanjay on 5/8/18.
//
int main(int argc, char **argv) {
    Controller vjoy(3,2,0x044f,0x0402,"Thrustmaster Warthog Joystick");
    system("../wejoy ../../scripts/example.lua");
    return 0;
}
