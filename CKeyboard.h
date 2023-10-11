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

#ifndef CKEYBOARD_HEADER
#define CKEYBOARD_HEADER

#include <string>
#include <set>

class CKeyboardEvent
{
public:
  static const int RELEASED = 0;
  static const int PRESSED = 1;
  static const int REPEATED = 2; // Not used
  int state;
  char code;
};

class CKeyboard
{
private:
  int fd = -1;
  std::string eventPath;

public:
  std::set<int> pressedKeys;
  bool call_kbd_down_LuaFunction = false;

  std::string getEventPath();
  bool isOpen();
  bool readEvent(CKeyboardEvent*);
        
  CKeyboard(std::string eventPath);
  ~CKeyboard();
};

#endif
