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

#ifndef CVIRTUAL_JOY_H
#define CVIRTUAL_JOY_H

#include <cstdint>
#include <linux/input.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include "suinput.h"


class CVirtualJoy
{
public:
  struct ABSData
  {
    uint16_t code;
    input_absinfo absinfo;
  };

private:
  int deviceid;
  int uinput_fd;

  std::string name;
  uint16_t vendorid;
  uint16_t productid;

  std::vector<uint16_t> available_buttons;
  std::vector<ABSData> available_abs;
  std::vector<uint16_t> available_rels;

  std::array<uint16_t, KEY_CNT> button_data;
  std::array<uint16_t, ABS_CNT> abs_data;
  std::array<uint16_t, REL_CNT> rel_data;

public:

  const std::string get_name() { return name;}
  uint8_t get_button_status(uint16_t t) { return button_data[t];}
  uint32_t get_abs_status(uint16_t t){ return abs_data[t]; }
  uint32_t get_rel_status(uint16_t t){ return rel_data[t]; }
  size_t get_num_buttons() { return available_buttons.size();}
  size_t get_num_abs() { return available_abs.size();}
  size_t get_num_rels() { return available_rels.size();}

  void send_button_event(uint16_t, uint8_t);
  void send_abs_event(uint16_t, uint32_t);
  void send_rel_event(uint16_t, uint32_t);
  int get_device_id();
  bool is_open();
  void print_verbose_info();
  CVirtualJoy(const std::vector<uint16_t>& _buttons, const std::vector<ABSData>& _abs, const std::vector<uint16_t>& _rels, std::string _name = "", uint16_t _vendorid = 0xFED1, uint16_t _productid = 0x001);
  ~CVirtualJoy();

};

#endif
