#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include "CKeyboard.h"
#include "joystick.h"
#include "CVirtualJoy.h"
#include "CVirtualKeyboard.h"

namespace GLOBAL
{
  std::vector<Joystick*>    joyList;
  std::vector<CVirtualJoy*> vJoyList;
  std::vector<CKeyboard*>   kbdList;
  CVirtualKeyboard*         vKeyboard;
}

#endif
