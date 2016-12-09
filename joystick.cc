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

#include "joystick.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include "unistd.h"

#include <stdexcept>
#include <cstring>
#include <libudev.h>
#include <algorithm> //find

#include <sys/ioctl.h>

#define KEY_MAX 				0x2ff
#define BTN_MISC				0x100
typedef unsigned char   		__u8;
typedef uint16_t        		__u16;
#define JSIOCGAXES              _IOR('j', 0x11, __u8)                           /* get number of axes */
#define JSIOCGBUTTONS           _IOR('j', 0x12, __u8)							/* get number of buttons */
#define JSIOCGBTNMAP            _IOR('j', 0x34, __u16[KEY_MAX - BTN_MISC + 1])  /* get button mapping */
#define JSIOCGAXMAP             _IOR('j', 0x32, __u8[ABS_CNT])					/* get axis mapping */


Joystick::Joystick(int joystickNumber)
{
	
	if(!retrieveID(joystickNumber, vendorid, productid)) return;
	joyNum = joystickNumber;

	std::stringstream sstm;
	sstm << "/dev/input/js" << joystickNumber;
	openPath(sstm.str());

	_get_joystick_mapping(buttonMappings, axisMappings);
	axesData.resize(axisMappings.size(), 0);
}

Joystick::Joystick(int _vendorid, int _productid)
{
	std::vector<unsigned int> lJSDevices;
	std::string dir("/dev/input/");

	//Open '/dev/input' directory
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(dir.c_str())) == NULL)
	{
        std::cout << "Error(" << errno << ") opening " << dir << '\n';
        return;
    }

	//Read '/dev/input' directory
    while ((dirp = readdir(dp)) != NULL)
	{
    	std::string cFile(dirp->d_name);
		//If a file that begins with 'js' is found
		if(cFile.compare(0, 2, "js") == 0)
		{
			cFile = cFile.substr(2, std::string::npos);
			int num;
			//Add the trailing number to the device list after js if the number is valid
			try
			{
				num = std::stoi(cFile);
			}
			catch(...)
			{
				continue;
			}
			lJSDevices.push_back(num);
		}//if
    }
    closedir(dp);


	for(unsigned int i=0; i<lJSDevices.size(); i++)
	{
		if(!retrieveID(lJSDevices[i], vendorid, productid)) continue;
		if(vendorid == _vendorid && productid == _productid)
		{
			joyNum = lJSDevices[i];
			openPath("/dev/input/js" + std::to_string(joyNum));
			_get_joystick_mapping(buttonMappings, axisMappings);
			axesData.resize(axisMappings.size(), 0);
			break;
		}//if
	}//for
}

void Joystick::openPath(std::string devicePath)
{
	_fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
	_devicePath = devicePath;
	
}

void Joystick::setPath(std::string devicePath)
{
	_devicePath = devicePath;
}

void Joystick::openJoy()
{
	_fd = open(_devicePath.c_str(), O_RDONLY | O_NONBLOCK);
}

void Joystick::closeJoy()
{
	close(_fd);
	_fd = -1;
}

const uint64_t Joystick::get_button_flags()
{
	return buttonFlags;
}


const uint64_t Joystick::get_axes_notify_flags()
{
	return axesNotifyFlags;
}

int Joystick::get_button_status(int type)
{
//	uint64_t check = (value) ? (get_button_flags() | 1ul << type) : (get_button_flags() & ~(1uL << type));
//	if(check==get_button_flags()) return;	//Button already set, we do not need to emit the data again

	return (buttonFlags & (1ul << type) ? true : false);
}

int Joystick::get_axis_status(int _i)
{
	return axesData[_i];
}



bool Joystick::readJoy(JoystickEvent* event)
{
  int bytes = read(_fd, event, sizeof(*event));


  if (bytes == -1)
    return false;


	if(event->type & JS_EVENT_BUTTON)
	{
		buttonFlags = (event->value) ? (buttonFlags | 1ul << event->number) : (buttonFlags & ~(1uL << event->number));
	}//if
	else if(event->type & JS_EVENT_AXIS)
	{
		axesData[event->number] = event->value;
		axesNotifyFlags = 1uL << event->number;
	}//if


  // NOTE if this condition is not met, we're probably out of sync and this
  // Joystick instance is likely unusable
  return bytes == sizeof(*event);
}

void Joystick::_get_joystick_mapping(std::vector<int>& _buttons, std::vector<int>& _axes)
{
	uint8_t num_button = 0;	
	ioctl(_fd, JSIOCGBUTTONS, &num_button);
	uint8_t button_count = num_button;

	uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
	if (ioctl(_fd, JSIOCGBTNMAP, btnmap) < 0)
	{
		std::ostringstream str;
		str << _devicePath << ": " << std::strerror(errno);
		throw std::runtime_error(str.str());
	}
	else
	{
		std::copy(btnmap, btnmap + button_count, std::back_inserter(_buttons));
	}



	uint8_t num_axis = 0;
	ioctl(_fd, JSIOCGAXES, &num_axis);
	uint8_t axis_count = num_axis;

	uint8_t axismap[ABS_MAX + 1];
	if (ioctl(_fd, JSIOCGAXMAP, axismap) < 0)
	{
		std::ostringstream str;
		str << _devicePath << ": " << strerror(errno);
		throw std::runtime_error(str.str());
	}
	else
	{
		std::copy(axismap, axismap + axis_count, std::back_inserter(_axes));
	}
}


int Joystick::get_button_index(int _type)
{
	std::vector<int>::iterator it;
	it = std::find(buttonMappings.begin(), buttonMappings.end(), _type);
	if(it == buttonMappings.end()) return -1;
	
	return it - buttonMappings.begin();
}

int Joystick::get_axis_index(int _type)
{
	std::vector<int>::iterator it;
	it = std::find(axisMappings.begin(), axisMappings.end(), _type);
	if(it == axisMappings.end()) return -1;
	
	return it - axisMappings.begin();
}


unsigned int Joystick::get_num_buttons()
{
	return buttonMappings.size();
}

unsigned int Joystick::get_num_axes()
{
	return axisMappings.size();
}


bool Joystick::isFound()
{
  return _fd >= 0;
}

Joystick::~Joystick()
{
  if(_fd>=0) close(_fd);
};


/*
#define MAX_DEVICES 32
void Joystick::retrieveJSPaths(int* indexes)
{
	int k = 0;
	while(k<MAX_DEVICES)
	{
		std::stringstream path;
		path << "/dev/input/js" << k;
		std::ifstream in(path.str().c_str());
		if(!in) break;
		indexes[k] = k;
		k++;
	}//while
	indexes[k] = -1;
}
*/

bool Joystick::retrieveID(int index, int& _vendorid, int& _productid)
{
	// Use udev to look up the product and manufacturer IDs
	struct udev *udev = udev_new();
	if (udev)
	{
		char sysname[32];
		std::snprintf(sysname, sizeof(sysname), "js%u", index);
		struct udev_device *dev = udev_device_new_from_subsystem_sysname(udev, "input", sysname);
		dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
		if (!dev)
		{
			std::cout << "Unable to find parent USB device index " << index << std::endl;
			udev_unref(udev);
			return false;
		}
		
		const std::string s_vendorid = udev_device_get_sysattr_value(dev, "idVendor");
		const std::string s_productid = udev_device_get_sysattr_value(dev, "idProduct");
		_vendorid = strtoul(s_vendorid.c_str(), NULL, 16);
		_productid = strtoul(s_productid.c_str(), NULL, 16);

		udev_device_unref(dev);
		udev_unref(udev);
	}
	else
	{
		std::cout << "Cannot create udev" << std::endl;
		return false;
	}

	return true;
}


