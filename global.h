#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <map>
#include "joystick.h"
#include "CVirtualJoy.h"
#include "CVirtualKeyboard.h"

namespace GLOBAL {
    std::vector<std::string> wiimoteList;
    std::map<std::string,Joystick *> joyList;
    std::map<std::string,CVirtualJoy *> vJoyList;
    CVirtualKeyboard vKeyboard;
}

#endif
