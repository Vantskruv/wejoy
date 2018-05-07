// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#include "joystick.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include "unistd.h"
#include <libevdev-1.0/libevdev/libevdev.h>

#include <stdexcept>
#include <cstring>
#include <libudev.h>
#include <algorithm> //find

#include <sys/ioctl.h>

#define KEY_MAX                0x2ff
#define BTN_MISC                0x100
typedef unsigned char __u8;
typedef uint16_t __u16;
#define JSIOCGAXES              _IOR('j', 0x11, __u8)                           /* get number of axes */
#define JSIOCGBUTTONS           _IOR('j', 0x12, __u8)                            /* get number of buttons */
#define JSIOCGBTNMAP            _IOR('j', 0x34, __u16[KEY_MAX - BTN_MISC + 1])  /* get button mapping */
#define JSIOCGAXMAP             _IOR('j', 0x32, __u8[ABS_CNT])                    /* get axis mapping */

Joystick::Joystick(LuaStick stick) {
    int current = 0;
    std::string dir("/dev/input/");

    //Open '/dev/input' directory
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << '\n';
        return;
    }
    //Read '/dev/input' directory
    while ((dirp = readdir(dp)) != NULL) {
        std::string cFile(dirp->d_name);
        //If a file that begins with 'event' is found
        if (cFile.compare(0, 5, "event") == 0) {
            openPath("/dev/input/" + cFile);
            int rc = 1;
            struct libevdev *_dev = NULL;
            rc = libevdev_new_from_fd(_fd, &_dev);
            if (rc < 0) {
                fprintf(stderr, "Skipping device /dev/input/%s, as it reported an error: (%s)\n", dirp->d_name, strerror(-rc));
                //skip invalid devices
                closeJoy();
                continue;
            }
            name = libevdev_get_name(_dev);
            int vid = libevdev_get_id_vendor(_dev);
            int pid = libevdev_get_id_product(_dev);
            if ((vid == stick.vendor_id && pid == stick.product_id) || name == stick.name) {
                if (current == stick.index) {
                    dev = _dev;
                    lua_name = stick.lua_name;
                    break;
                }
                current++;
            }//if
            closeJoy();
        }//if
    }
    closedir(dp);
}

void Joystick::openPath(std::string devicePath) {
    _fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    _devicePath = devicePath;
}

void Joystick::setPath(std::string devicePath) {
    _devicePath = devicePath;
}

void Joystick::openJoy() {
    _fd = open(_devicePath.c_str(), O_RDONLY | O_NONBLOCK);
}

void Joystick::closeJoy() {
    close(_fd);
    _fd = -1;
}

const uint64_t Joystick::get_button_flags() {
    return buttonFlags;
}


const uint64_t Joystick::get_axes_notify_flags() {
    return axesNotifyFlags;
}

int Joystick::get_button_status(int type) {
    return libevdev_get_event_value(dev, EV_KEY, static_cast<unsigned int>(type));
}

int Joystick::get_axis_status(int _i) {
    return libevdev_get_event_value(dev, EV_ABS, static_cast<unsigned int>(_i));
}


unsigned int Joystick::get_num_buttons() {
    return buttonMappings.size();
}

unsigned int Joystick::get_num_axes() {
    return axisMappings.size();
}


bool Joystick::isFound() {
    return _fd >= 0;
}

Joystick::~Joystick() {
    if (_fd >= 0) close(_fd);
};

