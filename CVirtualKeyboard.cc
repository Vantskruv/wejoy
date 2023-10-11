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

#include "CVirtualKeyboard.h"
#include "suinput.h"
#include <cstring>  //memset
#include <iostream> //cout


CVirtualKeyboard::CVirtualKeyboard()
{
  //Get a device descriptor
  fd = suinput_open();
  if(fd<0)
    {
      std::cout << "ERROR " << fd << ": Failed opening virtual keyboard device.\n";
      return;
    }//if


  //Setup keys for device
  for(unsigned int i=1; i<120; i++)
    {
      if(suinput_enable_event(fd, EV_KEY, i)<0)
	{
	  std::cout << "ERROR: Failed enabling event for key " << i << " on virtual keyboard device.\n";
	}//if
    }//if
	

  //Create and initialize the device
  struct uinput_user_dev user_dev;
  memset(&user_dev, 0, sizeof(struct uinput_user_dev));
  std::string dName = "WeJoy Virtual Keyboard";
  strcpy(user_dev.name, dName.c_str());
  user_dev.id.bustype = BUS_VIRTUAL; //TODO Should we use BUS_VIRTUAL instead BUS_USB?
  //user_dev.id.vendor = 0x06a3;
  //user_dev.id.product = 0x0764;
  user_dev.id.version = 1;
  int r = suinput_create(fd, &user_dev);
  if(r)
    {
      std::cout <<"ERROR " << r << ": Failed creating virtual keyboard device.\n";
      suinput_destroy(fd);
      fd = -1;
      return;
    }//if

  std::cout << "Successfully created virtual keyboard device.\n";
}


CVirtualKeyboard::~CVirtualKeyboard()
{
  suinput_destroy(fd);
}

bool CVirtualKeyboard::isOpen()
{
  return fd>=0;
}

int CVirtualKeyboard::get_key_status(int type)
{
  return 0;
  //return (buttonFlags & (1ul << type) ? true : false);
}


void CVirtualKeyboard::send_key_event(int type, int value)
{
  //uint64_t check = (value) ? (get_button_flags() | 1ul << type) : (get_button_flags() & ~(1uL << type));	//Set new button flags state
  //if(check==get_button_flags()) return;	//Button already set, we do not need to emit the data again
  //set_button_flags(check);				//Updating button flags
	
  suinput_emit(fd, EV_KEY, type, value);
  suinput_syn(fd);
}
