#include "controller.h"
#include "../buttons_ref.h"
#include "../suinput.h"
#include <cstring>    //memset
#include <string>
#include <iostream> //cout
#include <dirent.h>

static int static_deviceid = 0;        //Device incrementer

Controller::Controller(unsigned int _buttons, unsigned int _axes, __u16 vid, __u16 pid, std::string name) {
    deviceid = static_deviceid;

    std::cout << "Opening virtual device " << deviceid << ".\n";
    //Get a device descriptor
    fd = suinput_open();
    if (fd < 0) {
        std::cout << "ERROR " << fd << ": Failed opening virtual device " << deviceid << ".\n";
        return;
    }//if


    //Setup buttons for device
    if (_buttons >= BUTTONS_SIZE)
        std::cout << "WARNING: Number of buttons (" << _buttons << ") for virtual device " << deviceid
                  << " exceeds maximum allowable buttons which is " << (BUTTONS_SIZE - 1) << ".\n";
    for (unsigned int i = 0; i < _buttons && i < BUTTONS_SIZE; i++) {
        if (suinput_enable_event(fd, EV_KEY, buttons_ref::BUTTONS[i]) < 0) {
            std::cout << "ERROR: Failed enabling event for button " << i << " on virtual device << " << deviceid
                      << ".\n";
        }//if
    }//if

    //Setup axesMapping for device

    if (_axes >= AXES_SIZE)
        std::cout << "WARNING: Number of axesMapping (" << _axes << ") for virtual device " << deviceid
                  << " exceeds maximum allowable axesMapping which is " << (AXES_SIZE - 1) << ".\n";
    for (unsigned int i = 0; i < _axes && i < AXES_SIZE; i++) {
        if (suinput_enable_event(fd, EV_ABS, buttons_ref::AXES[i]) < 0) {
            std::cout << "ERROR: Failed enabling event for axis " << i << " on virtual device << " << deviceid << ".\n";
        }
    }//if
    axesData.resize(_axes, 0);

    std::cout << "Creating virtual device " << deviceid << ".\n";
    //Create and initialize the device
    struct uinput_user_dev user_dev;
    memset(&user_dev, 0, sizeof(struct uinput_user_dev));
    strcpy(user_dev.name, name.c_str());

    for (unsigned int i = 0; i < _axes && i < AXES_SIZE; i++) {
        user_dev.absmax[i] = MAX_ABS_VAL;
        user_dev.absmin[i] = MIN_ABS_VAL;
    }

    user_dev.id.bustype = BUS_VIRTUAL;
    user_dev.id.vendor = vid;
    user_dev.id.product = pid;
    user_dev.id.version = 1;
    int r = suinput_create(fd, &user_dev);
    if (r) {
        std::cout << "ERROR " << r << ": Failed creating virtual device " << deviceid << ".\n";
        suinput_destroy(fd);
        fd = -1;
        return;
    }
    std::cout << "Created virtual device " << name << ".\n";
    std::cout << "Successfully created virtual device " << deviceid << ".\n";
    static_deviceid++;
}


Controller::~Controller() {
    suinput_destroy(fd);
}

bool Controller::isOpen() {
    return fd >= 0;
}

int Controller::getDeviceid() {
    return deviceid;
}


void Controller::send_button_event(int type, int value) {
    uint64_t check = (value) ? (get_button_flags() | 1ul << type) : (get_button_flags() &
                                                                     ~(1uL << type));    //Set new button flags state
    if (check == get_button_flags()) return;    //Button already set, we do not need to emit the data again
    set_button_flags(check);                //Updating button flags

    suinput_emit(fd, EV_KEY, buttons_ref::BUTTONS[type], value);
    suinput_syn(fd);
}

void Controller::send_axis_event(int type, int value) {
    set_axis_data(type, value);
    suinput_emit(fd, EV_ABS, buttons_ref::AXES[type], value);
    suinput_syn(fd);
}
