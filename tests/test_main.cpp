#include <dirent.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <cstring>
#include <fcntl.h>
#include <zconf.h>
#include <csignal>
#include <wait.h>
#include "controller.h"

#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "buttons_ref.h"
//
// Created by sanjay on 5/8/18.
//

struct libevdev *searchForJoyStick(const std::string &_name) {
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
            int rc;
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

void next_event() {
    //Ask for the next event, so we can can check it.
    libevdev_next_event(keyboard, LIBEVDEV_READ_FLAG_NORMAL, &ev_k);
    libevdev_next_event(controller, LIBEVDEV_READ_FLAG_NORMAL, &ev_c);
    while (libevdev_has_event_pending(keyboard) && ev_k.type == EV_SYN) {
        libevdev_next_event(keyboard, LIBEVDEV_READ_FLAG_NORMAL, &ev_k);
    }
    while (libevdev_has_event_pending(controller) && ev_c.type == EV_SYN) {
        libevdev_next_event(controller, LIBEVDEV_READ_FLAG_NORMAL, &ev_c);
    }
//    printf("Event: %s %s %i %i %d\n",
//           libevdev_event_type_get_name(ev_c.type),
//           libevdev_event_code_get_name(ev_c.type, ev_c.code),
//           ev_c.type, ev_c.code,
//           ev_c.value);
}

SCENARIO("Joystick Buttons work", "[example.lua]") {
    GIVEN("example.lua with a joystick and a throttle") {
        WHEN("The first button on the throttle is pressed") {
            vthrottle->send_button_event(0, 1);
            sleep(1);
            THEN("expect the a key to be pressed") {
                next_event();
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_A) == 1);
            }
            std::cout << std::endl;
        }
        WHEN("The first button on the throttle is released") {
            vthrottle->send_button_event(0, 0);
            sleep(1);
            THEN("expect the a key to be released") {
                next_event();
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_A) == 0);
            }
        }
        WHEN("The second button on the throttle is pressed") {
            vthrottle->send_button_event(1, 1);
            sleep(1);
            THEN("expect the a key to be pressed and the shift key") {
                next_event();
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_LEFTSHIFT) == 1);
                next_event();
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_A) == 1);
            }
            std::cout << std::endl;
        }
        WHEN("The second button on the throttle is released") {
            vthrottle->send_button_event(1, 0);
            sleep(1);
            THEN("expect the a key to be pressed and the shift key") {
                next_event();
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_LEFTSHIFT) == 0);
                next_event();
                REQUIRE(libevdev_get_event_value(keyboard, EV_KEY, KEY_A) == 0);
            }
        }

        WHEN("The first button on the joystick is pressed") {
            vjoy->send_button_event(0, 1);
            sleep(1);
            THEN("expect the first button on the output joystick to be pressed") {
                next_event();
                REQUIRE(libevdev_get_event_value(controller, EV_KEY, buttons_ref::BUTTONS[0]) == 1);
            }
        }
        WHEN("The first button on the joystick is released") {
            vjoy->send_button_event(0, 0);
            sleep(1);
            THEN("expect the first button on the output joystick to be released") {
                next_event();
                REQUIRE(libevdev_get_event_value(controller, EV_KEY, buttons_ref::BUTTONS[0]) == 0);
            }
        }

        WHEN("The third button on the joystick is pressed") {
            vjoy->send_button_event(2, 1);
            sleep(1);
            THEN("expect the third button on the output joystick to be pressed") {
                next_event();
                REQUIRE(libevdev_get_event_value(controller, EV_KEY, buttons_ref::BUTTONS[2]) == 1);
            }
        }
        WHEN("The third button on the joystick is released") {
            vjoy->send_button_event(2, 0);
            sleep(1);
            THEN("expect the third button on the output joystick to be released") {
                next_event();
                REQUIRE(libevdev_get_event_value(controller, EV_KEY, buttons_ref::BUTTONS[2]) == 0);
            }
        }
        WHEN("The first button on the joystick is released") {
            THEN("expect the first axis to equal the input first axis") {
                vjoy->send_button_event(1, 0);
                sleep(1);
                next_event();
                next_event();
                for (int i = MIN_ABS_VAL; i < MAX_ABS_VAL; i += 1000) {
                    vjoy->send_axis_event(0, i);
                    usleep(1000);
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_ABS, buttons_ref::AXES[0]) == i);
                }
            }
        }

        WHEN("The second button on the joystick is pressed") {
            THEN("expect the second axis to equal the input second axis but inverted") {
                vjoy->send_button_event(1, 1);
                sleep(1);
                next_event();
                next_event();
                next_event();
                for (int i = MIN_ABS_VAL; i < MAX_ABS_VAL; i += 1000) {
                    vjoy->send_axis_event(1, i);
                    usleep(1000);
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_ABS, buttons_ref::AXES[1]) == -i);
                }
            }
        }

        WHEN("The second button on the joystick is released") {
            THEN("expect the first axis to equal the input first axis") {
                vjoy->send_button_event(1, 0);
                sleep(1);
                next_event();
                next_event();
                next_event();
                for (int i = MIN_ABS_VAL; i < MAX_ABS_VAL; i += 1000) {
                    vjoy->send_axis_event(0, i);
                    usleep(1000);
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_ABS, buttons_ref::AXES[0]) == i);
                }
            }
        }



        WHEN("The second button on the joystick is pressed") {
            THEN("expect the second axis to equal the input second axis, but inverted") {
                vjoy->send_button_event(1, 1);
                sleep(1);
                next_event();
                next_event();
                next_event();
                for (int i = MIN_ABS_VAL; i < MAX_ABS_VAL; i += 1000) {
                    vjoy->send_axis_event(1, i);
                    usleep(1000);
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_ABS, buttons_ref::AXES[1]) == -i);
                }
            }
        }


        WHEN("The second button on the joystick is pressed") {
            THEN("expect the first axis and second axis to be an inverted version of the input joystick's first and second axis") {
                for (int i = MIN_ABS_VAL; i < MAX_ABS_VAL; i += 1000) {
                    vjoy->send_axis_event(0, i);
                    vjoy->send_axis_event(1, i);
                    vjoy->send_button_event(1, 1);
                    usleep(1000);
                    next_event();
                    next_event();
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_KEY, buttons_ref::BUTTONS[1]) == 1);
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_ABS, buttons_ref::AXES[0]) == -i);
                    next_event();
                    REQUIRE(libevdev_get_event_value(controller, EV_ABS, buttons_ref::AXES[1]) == -i);
                    vjoy->send_button_event(1, 0);
                    usleep(1000);
                    next_event();
                    next_event();
                    next_event();
                }
            }
        }
    }

}

int main(int argc, char *argv[]) {
    std::cout
            << "Note: this application WILL emulate a keyboard. This means you should try to avoid doing anyting with your computer while this test runs."
            << std::endl;
    vjoy = new Controller(3, 2, 0x044f, 0x0402, "Thrustmaster Warthog Joystick");
    vthrottle = new Controller(3, 2, 0x044f, 0x0404, "Thrustmaster Warthog Throttle");
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {const_cast<char *>("../wejoy"), const_cast<char *>("../../scripts/example.lua"), nullptr};
        execv(args[0], args);
    } else {
        do {
            keyboard = searchForJoyStick("WeJoy Virtual Keyboard");
        } while (keyboard == nullptr);
        controller = searchForJoyStick("WeJoy Virtual Device 0");


        int result = Catch::Session().run(argc, argv);
        kill(pid, SIGKILL);
        int status = 0;
        wait(&status);

        return result;
    }

    return 0;
}