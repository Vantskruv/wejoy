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

#include <string>
#include <vector>
#include <linux/input.h> //EV_KEY, EV_ABS

#define JS_EVENT_BUTTON 0x01 // button pressed/released
#define JS_EVENT_AXIS   0x02 // joystick moved
#define JS_EVENT_INIT   0x80 // initial state of device

class JoystickEvent      // Encapsulates all data relevant to a sampled joystick event.
{
public:
  unsigned int time;     // The timestamp of the event, in milliseconds.
  short value;           // The value associated with this joystick event.
                         // For buttons this will be either 1 (down) or 0 (up).
                         // For axes, this will range between -32768 and 32767.
  unsigned char type;    // The event type.
  unsigned char number;  // The axis/button number.

  bool isButton()        // Returns true if this event is the result of a button press.
  {
    return (type & JS_EVENT_BUTTON) != 0;
  }


  bool isAxis()          // Returns true if this event is the result of an axis movement.
  {
    return (type & JS_EVENT_AXIS) != 0;
  }

   bool isInitialState() // Returns true if this event is part of the initial state obtained when the joystick is first connected to.
  {
    return (type & JS_EVENT_INIT) != 0;
  }
};


class Joystick           // Represents a joystick device. Allows data to be sampled from it.
{
private:
  void openPath(std::string);
  int _fd = -1;
  unsigned int joyNum;
  int vendorid;
  int productid;
  std::string _devicePath;
  uint64_t buttonFlags = 0;     //Curent values of all buttons
  std::vector<int> axesData;    //Current values of all axis
  uint64_t axesNotifyFlags = 0; //Tells if values are changed from last read data
  std::vector<int> buttonMappings;
  std::vector<int> axisMappings;


  void _get_joystick_mapping(std::vector<int>&, std::vector<int>&);
  
public:
  ~Joystick();

  Joystick(int joystickNumber);                 // Initialises an instance for the joystick with the specified, zero-indexed number.
  Joystick(int, int, std::vector<Joystick*>);
  Joystick(std::string devicePath);             // Initialises an instance for the joystick device specified.

  bool           isFound();                     // Returns true if the joystick was found and may be used, otherwise false.
  bool           readJoy(JoystickEvent* event); // Attempts to populate the provided JoystickEvent instance with data from the joystick. Returns true if data is available, otherwise false.
  void           openJoy();
  void           closeJoy();
  void           setPath(std::string);
  unsigned int   getJoyNum(){return joyNum;};
  int            getVendorID(){return vendorid;};
  int            getProductID(){return productid;};
  std::string    getDevicePath(){return _devicePath;};
  void           addListener(void(*)(void*, int, int, int), int, int); //Listener, event and type
  unsigned int   get_num_buttons();
  unsigned int   get_num_axes();
  int            get_button_index(int);
  int            get_axis_index(int);
  const uint64_t get_button_flags();
  int            get_button_status(int);
  int            get_axis_status(int);
  const uint64_t get_axes_notify_flags();
  int            get_axis_value(int _i);

  static bool retrieveID(int, int&, int&);
};

#endif
