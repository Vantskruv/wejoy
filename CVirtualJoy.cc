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

#include "CVirtualJoy.h"
#include "suinput.h"
#include <cstring>               //memset
#include <iostream>              //cout
#include <libevdev-1.0/libevdev/libevdev.h>

static int static_deviceid = 0;  //Device incrementer

CVirtualJoy::CVirtualJoy(const std::vector<uint16_t>& _buttons, const std::vector<ABSData>& _abs, const std::vector<uint16_t>& _rel, std::string _name, uint16_t _vendorid, uint16_t _productid)
: name(_name), vendorid(_vendorid), productid(_productid)
{
  //Get a device descriptor
  uinput_fd = suinput_open();
  if(uinput_fd<0)
    {
      std::cout << "ERROR: Failed opening virtual device.\n";
      return;
    }//if

  //Setup buttons for device
  for(const auto& button : _buttons)
  {
    if(suinput_enable_event(uinput_fd, EV_KEY, button) < 0)
    {
      std::cout << "ERROR: Failed enabling event for virtual button " << button << "\n";
      continue;
    }//if
    available_buttons.push_back(button);
  }//for

  //Setup abs axes for device
  for(const auto& abs : _abs)
  {
    if(suinput_enable_event(uinput_fd, EV_ABS, abs.code)<0)
    {
      std::cout << "ERROR: Failed enabling event for virtual absolute axis " << abs.code << ".\n";
      continue;
    }//if
    available_abs.push_back(abs);
  }//for

  //Setup rel axes for device
  for(const auto& rel : _rel)
  {
    if(suinput_enable_event(uinput_fd, EV_REL, rel)<0)
    {
      std::cout << "ERROR: Failed enabling event for virtual relative axis " << rel << ".\n";
      continue;
    }//if
    available_rels.push_back(rel);
  }//for

  //Create and initialize the device
  struct uinput_user_dev user_dev;

  for(const auto& abs : _abs)
  {
    user_dev.absmax[abs.code]=abs.absinfo.maximum;
    user_dev.absmin[abs.code]=abs.absinfo.minimum;
    user_dev.absflat[abs.code]=abs.absinfo.flat;
    user_dev.absfuzz[abs.code]=abs.absinfo.fuzz;
  }

  user_dev.id.bustype = BUS_USB;    // BUS_VIRTUAL can be used aswell.
  user_dev.id.vendor = vendorid;    // Use one of the unregistered ranges: 0xFE00–0xFEFF or 0xFF00–0xFFFF.
  user_dev.id.product = productid;  // Can be used more freely, as it lies under vendorid

  user_dev.id.version = 1;
  memset(&user_dev, 0, sizeof(struct uinput_user_dev));
  deviceid = static_deviceid++;
  if(name.empty()) name = "WeJoy Virtual Device " + std::to_string(deviceid);
  strcpy(user_dev.name, name.c_str());

  int r = suinput_create(uinput_fd, &user_dev);
  if(r)
    {
      std::cout <<"ERROR " << r << ": Failed creating virtual device.\n";
      suinput_destroy(uinput_fd);
      uinput_fd = -1;
      return;
    }//if
}


CVirtualJoy::~CVirtualJoy()
{
  if(uinput_fd>=0) suinput_destroy(uinput_fd);
}

bool CVirtualJoy::is_open()
{
  return uinput_fd>=0;
}

int CVirtualJoy::get_device_id()
{
  return deviceid;
}

void CVirtualJoy::send_button_event(uint16_t code, uint8_t value)
{
  button_data[code] = value;
  suinput_emit(uinput_fd, EV_KEY, code, value);
  suinput_syn(uinput_fd);
}

void CVirtualJoy::send_abs_event(uint16_t code, uint32_t value)
{
  abs_data[code] = value;
  suinput_emit(uinput_fd, EV_ABS, code, value);
  suinput_syn(uinput_fd);
}

void CVirtualJoy::send_rel_event(uint16_t code, uint32_t value)
{
  rel_data[code] = value;
  suinput_emit(uinput_fd, EV_REL, code, value);
  suinput_syn(uinput_fd);
}

/*
void CVirtualJoy::send_event(uint8_t type, uint16_t code, uint32_t value)
{
  data[type][code] = value;
  suinput_emit(uinput_fd, type, code, value);
  suinput_syn(uinput_fd);
}
*/

void CVirtualJoy::print_verbose_info()
{
  if(!is_open()) return;

  printf("Name: %s\n", name.c_str());
  printf("Vendorid: %.4x\n", vendorid);
  printf("Productid: %.4x\n", productid);

  printf("\nBTN_count: %lu\n", get_num_buttons());
  for(const auto& btn : available_buttons)
  {
    printf("%4d : %24s\n", btn, libevdev_event_code_get_name(EV_KEY, btn));
  }

  printf("\nREL_count: %lu\n", get_num_rels());
  for(const auto& rel : available_rels)
  {
    printf("%4d : %24s\n", rel, libevdev_event_code_get_name(EV_REL, rel));
  }

  printf("\nABS_count: %lu\n", get_num_abs());
  for(const auto& abs : available_abs)
  {
    printf("%4d : %24s | Min: %6d | Max: %6d | Fuzz: %6d | Flat: %6d\n",
    abs.code, libevdev_event_code_get_name(EV_ABS, abs.code), abs.absinfo.minimum, abs.absinfo.maximum, abs.absinfo.fuzz, abs.absinfo.flat);
  }
}

