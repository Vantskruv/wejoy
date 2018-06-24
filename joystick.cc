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
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "unistd.h"
#include <libevdev-1.0/libevdev/libevdev.h>

#include <cstring>
#include <algorithm>
#include <utility>
#include <libudev.h>

Joystick::Joystick(LuaStick stick, std::map<int,std::string> &wiimoteList) {
    this->stick = std::move(stick);
    init(wiimoteList);
}

void Joystick::initMaps() {
    std::map<int, const char *>::iterator it;
    for (int i = 0; i < ABS_MAX; i++) {
        if (libevdev_has_event_code(dev, EV_ABS, i)) {
            axisMappingsRev[i] = static_cast<int>(axisMappings.size());
            axisMappings.push_back(i);
        }
    }
    axesData.resize(axisMappings.size());
    for (int i = 0; i < KEY_MAX; i++) {
        if (libevdev_has_event_code(dev, EV_KEY, i)) {
            buttonMappingsRev[i] = static_cast<int>(buttonMappings.size());
            buttonMappings.push_back(i);
        }
    }
}

void Joystick::closeJoy(std::map<int,std::string> &wiimoteList) {
    close(_fd);
    _fd = -1;
    path = "";
    if (stick.wiimote != -1) {
        wiimoteList.erase(stick.wiimote);
    }
}

void Joystick::handleEvent(input_event ev) {
    if (ev.type == EV_KEY) {
        int index = get_button_index(ev.code);
        buttonFlags = (ev.value) ? (buttonFlags | 1ul << index) : (buttonFlags & ~(1uL << index));
    } else if (ev.type == EV_ABS) {
        int index = get_axis_index(ev.code);
        axesData[index] = ev.value;
    }
}

int Joystick::get_button_status(int type) {
    return (buttonFlags & (1ul << type)) != 0;
}

int Joystick::get_axis_status(int _i) {
    return axesData[_i];
}

int Joystick::get_axis_min(int _type) {
    return libevdev_get_abs_minimum(dev, static_cast<unsigned int>(_type));
}

int Joystick::get_axis_max(int _type) {
    return libevdev_get_abs_maximum(dev, static_cast<unsigned int>(_type));
}

int Joystick::get_button_index(int _type) {
    return buttonMappingsRev[_type];
}

int Joystick::get_axis_index(int _type) {
    return axisMappingsRev[_type];
}

int Joystick::get_button_type(int _index) {
    return buttonMappings[_index];
}

int Joystick::get_axis_type(int _index) {
    return axisMappings[_index];
}

bool Joystick::isFound() {
    return _fd >= 0;
}

Joystick::~Joystick() {
    if (_fd >= 0) close(_fd);
}

unsigned int Joystick::get_num_buttons() {
    return buttonMappings.size();
};

unsigned int Joystick::get_num_axes() {
    return axisMappings.size();
}

void Joystick::set_led_state(unsigned int index, bool state) {
    std::cout << libevdev_kernel_set_led_value(dev,index,state?LIBEVDEV_LED_ON:LIBEVDEV_LED_OFF) << std::endl;
}

std::string Joystick::getPath() {
    return path;
}

void Joystick::init(std::map<int,std::string> &wiimoteList) {
    int current = 0;
    std::string dir("/dev/input/");

    //Open '/dev/input' directory
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == nullptr) {
        std::cout << "Error(" << errno << ") opening " << dir << '\n';
        return;
    }
    struct udev *udev = udev_new();
    //Read '/dev/input' directory
    while ((dirp = readdir(dp)) != nullptr) {
        std::string cFile(dirp->d_name);
        //If a file that begins with 'event' is found
        if (cFile.compare(0, 5, "event") == 0) {
            std::string path = "/dev/input/" + cFile;
            int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
            struct libevdev *_dev = nullptr;
            int rc = libevdev_new_from_fd(fd, &_dev);
            if (rc < 0) {
//                fprintf(stderr, "Skipping device %s, as it reported an error: (%s)\n", path.c_str(), strerror(-rc));
                close(fd);
                continue;
            }

            name = libevdev_get_name(_dev);
            if (stick.wiimote != -1) {
                if (name.find("Nintendo Wii") == std::string::npos) {
                    close(fd);
                    continue;
                }
                struct udev_device *uudev = udev_device_new_from_subsystem_sysname(udev, "input", cFile.c_str());
                uudev = udev_device_get_parent_with_subsystem_devtype(uudev, "hid", NULL);
                if (!uudev) {
                    close(fd);
                    continue;
                }
                std::string devpath = udev_device_get_devpath(uudev);
                udev_device_unref(uudev);
                bool found = false;
                int idFound = false;
                for (auto &pair : wiimoteList) {
                    if (pair.second == devpath) {
                        found = true;
                    }
                    if (pair.first == stick.wiimote) {
                        idFound = true;
                    }
                }
                if (!found && idFound) {
                    close(fd);
                    continue;
                }
                if (!found) {
                    wiimoteList[stick.wiimote] = devpath;
                }
            }
            int vid = libevdev_get_id_vendor(_dev);
            int pid = libevdev_get_id_product(_dev);
            if ((vid == stick.vendor_id && pid == stick.product_id) || name == stick.name) {
                if (stick.index == -1 || current == stick.index) {
                    dev = _dev;
                    lua_name = stick.lua_name;
                    _fd = fd;
                    this->path = path;
                    libevdev_grab(_dev, LIBEVDEV_GRAB);
                    initMaps();
                    break;
                }
                current++;
            }
            close(fd);
        }
    }
    udev_unref(udev);
    closedir(dp);
};