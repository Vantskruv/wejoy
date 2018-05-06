#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include "joystick.h"
#include "CVirtualJoy.h"
#include "CVirtualKeyboard.h"

namespace GLOBAL {
    std::vector<Joystick *> joyList;
    std::vector<CVirtualJoy *> vJoyList;
    CVirtualKeyboard vKeyboard;
}

#endif
