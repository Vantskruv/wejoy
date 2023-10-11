/*
    Source part of project WeJoy
    Copyright (C) 2023 Johannes Bergmark

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
