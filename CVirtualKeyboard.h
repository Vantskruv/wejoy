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

#ifndef CVIRTUAL_KEYBOARD_H
#define CVIRTUAL_KEYBOARD_H

class CVirtualKeyboard
{
private:
  int fd;                                                //Device handler id
  //uint64_t keyFlags = 0;

public:
  int get_key_status(int);                               //Get defined key state
  //void set_key_flags(uint64_t _b){ keyFlags = _b; };   //Set keys to on or off 
  //const uint64_t& get_key_flags(){ return keyFlags; }; //Get the status of key states.


  void send_key_event(int, int);
  bool isOpen();
  CVirtualKeyboard();                                    //buttons, axes
  ~CVirtualKeyboard();

};

#endif
