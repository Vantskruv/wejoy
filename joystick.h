// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#ifndef __JOYSTICK_HH__
#define __JOYSTICK_HH__

#include <bitset>
#include <cstdint>
#include <linux/input.h>
#include <memory>
#include <string>
#include <vector>
#include <array>

struct JoystickEvent : input_event      // Encapsulates all data relevant to a sampled joystick event.
{
  bool isButton()        // Returns true if this event is the result of a button press.
  {
    return (type == EV_KEY);
  }

  bool isAxis()          // Returns true if this event is the result of an axis movement.
  {
    return type == EV_ABS;
  }

  bool isRel()
  {
    return type == EV_REL;
  }

/*
  bool isInitialState() // Returns true if this event is part of the initial state obtained when the joystick is first connected to.
  {
    return (type & JS_EVENT_INIT) != 0;
  }
*/
};


class Joystick           // Represents a joystick device. Allows data to be sampled from it.
{
private:
  struct libevdev* evdev = nullptr;
  int vendorid;
  int productid;
  std::string device_name;
  std::string device_path;
  std::string description;

  size_t number_of_buttons = 0;
  size_t number_of_abs = 0;
  size_t number_of_rels = 0;
  std::array<uint8_t, KEY_CNT> button_data;   //  Current valuesfof all keys/buttons
  std::array<int32_t, ABS_CNT> abs_data;      //  Current values of all axis
  std::array<int32_t, REL_CNT> rel_data;      //  Current values of all relative axes

public:
  ~Joystick();
  Joystick(const std::string& devicePath);

  bool            is_open();
  bool            read_joy(JoystickEvent* event); // Attempts to populate the provided JoystickEvent instance with data from the joystick. Returns true if data is available, otherwise false.
  bool            open_joy(const std::string& devicePath);
  void            close_joy();
  uint16_t        get_vendor_id(){return vendorid;};
  uint16_t        get_product_id(){return productid;};
  std::string     get_device_name() { return device_name; };
  std::string     get_device_path(){return device_path;};
  void            addListener(void(*)(void*, int, int, int), int, int); //Listener, event and type
  size_t          get_num_buttons();
  size_t          get_num_abs();
  size_t          get_num_rel();
  int             get_button_index(int);
  int             get_abs_index(int);
  int             get_button_status(int);
  int             get_abs_status(int);
  int             get_rel_status(int);
  void set_description(const std::string& _description) { description = _description;};
  const std::string& get_description() { return description;}
  void            print_verbose_info();
  
  struct SystemDeviceData
  {
    public:
      static std::vector<std::string> get_device_paths();
  };

};



#endif
