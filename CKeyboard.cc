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

#include "CKeyboard.h"
//#include <libudev.h>
//#include <stdio.h>
#include <string.h>
#include <unistd.h> //usleep
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>


CKeyboard::CKeyboard(std::string _eventPath)
{
  eventPath = _eventPath;
  fd = open(eventPath.c_str(), O_RDONLY | O_NONBLOCK);
  if(fd == -1)
    {
      fprintf(stderr, "KEYBOARD: Cannot open %s: %s.\n", eventPath.c_str(), strerror(errno));
    }
}

CKeyboard::~CKeyboard()
{
  if(fd>-1) close(fd);
}

std::string CKeyboard::getEventPath()
{
  return eventPath;
}

bool CKeyboard::isOpen()
{
  if(fd>-1) return true;

  return false;
}

bool CKeyboard::readEvent(CKeyboardEvent* _keyEvent)
{
  /*
    static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
    };
  */

  if(fd<0) return false;

  struct input_event ev;
  ssize_t n;
  n = read(fd, &ev, sizeof ev);
  if(n == ((ssize_t) - 1) || n!= sizeof(ev)) return false;

  if(ev.type == EV_KEY)
    {
      if(ev.value == 0) pressedKeys.erase(ev.code);
      else if(ev.value == 1) pressedKeys.insert(ev.code);
      else return false;
              
      _keyEvent->state = ev.value;
      _keyEvent->code = ev.code;
      //printf("%s 0x%04x (%d) %d\n", evval[ev.value], (int)ev.code, (int)ev.code, (ev.code == KEY_Q ? 1 : 0));
      return true;
    }

  return false;
}
