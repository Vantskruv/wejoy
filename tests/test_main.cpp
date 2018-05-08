#include <dirent.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <cstring>
#include <fcntl.h>
#include <zconf.h>
#include <signal.h>
#include <wait.h>
#include "controller.h"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
//
// Created by sanjay on 5/8/18.
//

struct libevdev *searchForJoyStick(const std::string _name) {
    struct libevdev *dev = nullptr;
    std::string dir("/dev/input/");

    //Open '/dev/input' directory
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == nullptr) {
        std::cout << "Error(" << errno << ") opening " << dir << '\n';
        return dev;
    }
    int fd;
    //Read '/dev/input' directory
    while ((dirp = readdir(dp)) != nullptr) {
        std::string cFile(dirp->d_name);
        //If a file that begins with 'event' is found
        if (cFile.compare(0, 5, "event") == 0) {
            fd = open(("/dev/input/" + cFile).c_str(), O_RDONLY | O_NONBLOCK);
            int rc = 1;
            struct libevdev *_dev = nullptr;
            rc = libevdev_new_from_fd(fd, &_dev);
            if (rc < 0) {
                fprintf(stderr, "Skipping device /dev/input/%s, as it reported an error: (%s)\n", dirp->d_name,
                        strerror(-rc));
                //skip invalid devices
                close(fd);
                continue;
            }
            std::string name = libevdev_get_name(_dev);
            if (name == _name) {
                dev = _dev;
                break;
            }//if
            close(fd);
        }//if
    }
    closedir(dp);
    return dev;
}

Controller *vjoy;
Controller *vthrottle;
struct libevdev *keyboard;
struct libevdev *controller;
struct input_event ev_k;
struct input_event ev_c;
void next_event(){
    libevdev_next_event(keyboard, LIBEVDEV_READ_FLAG_NORMAL, &ev_k);
    libevdev_next_event(controller, LIBEVDEV_READ_FLAG_NORMAL, &ev_c);
}
SCENARIO( "Joystick Buttons work", "[vector]" ) {
    GIVEN("example.lua with two joysticks") {
        WHEN("The first button on the throttle is pressed") {
            vthrottle->send_button_event(0,1);
            usleep(100);
            next_event();
            THEN("expect the a key to be pressed") {
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_A) == 1);
            }
        }
    }

}
int main( int argc, char* argv[] ) {
    vjoy = new Controller(3, 2, 0x044f, 0x0402, "Thrustmaster Warthog Joystick");
    vthrottle = new Controller(3, 2, 0x044f, 0x0404, "Thrustmaster Warthog Throttle");
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {const_cast<char *>("../wejoy"), const_cast<char *>("../../scripts/example.lua"), 0};
        execv(args[0], args);
    } else {
        do {
            keyboard = searchForJoyStick("WeJoy Virtual Keyboard");
        } while (keyboard == nullptr);
        controller = searchForJoyStick("WeJoy Virtual Device 0");


        int result = Catch::Session().run( argc, argv );
        kill( pid, SIGINT);
        int status = 0;
        wait(&status);

        return result;
    }

    return 0;
}