#include "CVirtualJoy.h"
#include "buttons_ref.h"
#include "suinput.h"
#include <cstring>	//memset
#include <iostream> //cout

static int static_deviceid = 0;		//Device incrementer

CVirtualJoy::CVirtualJoy(unsigned int _buttons, unsigned int _axes)
{
	deviceid = static_deviceid;
	
	std::cout << "Opening virtual device " << deviceid << ".\n";
	//Get a device descriptor
	fd = suinput_open();
	if(fd<0)
	{
		std::cout << "ERROR " << fd << ": Failed opening virtual device " << deviceid << ".\n";
		return;
	}//if


	//Setup buttons for device
	if(_buttons>=BUTTONS_SIZE) std::cout << "WARNING: Number of buttons (" << _buttons << ") for virtual device " << deviceid << " exceeds maximum allowable buttons which is " << (BUTTONS_SIZE - 1) << ".\n";
	for(unsigned int i=0; i<_buttons && i<BUTTONS_SIZE; i++)
	{
		if(suinput_enable_event(fd, EV_KEY, buttons_ref::BUTTONS[i])<0)
		{
			std::cout << "ERROR: Failed enabling event for button " << i << " on virtual device << " << deviceid << ".\n";
		}//if
	}//if

	//Setup axes for device
	
	if(_axes>=AXES_SIZE) std::cout << "WARNING: Number of axes (" << _axes << ") for virtual device " << deviceid << " exceeds maximum allowable axes which is " << (AXES_SIZE - 1) << ".\n";
	for(unsigned int i=0; i<_axes && i<AXES_SIZE; i++)
	{
		if(suinput_enable_event(fd, EV_ABS, buttons_ref::AXES[i])<0)
		{
			std::cout << "ERROR: Failed enabling event for axis " << i << " on virtual device << " << deviceid << ".\n";
		}
	}//if
	axesData.resize(_axes, 0);

	std::cout << "Creating virtual device " << deviceid << ".\n";
	//Create and initialize the device
	struct uinput_user_dev user_dev;
	memset(&user_dev, 0, sizeof(struct uinput_user_dev));
	std::string dName = "WeJoy Virtual Device " + std::to_string(deviceid);
	strcpy(user_dev.name, dName.c_str());

	for(unsigned int i=0; i<_axes && i<AXES_SIZE; i++) {
		user_dev.absmax[i]=32767;
		user_dev.absmin[i]=-32767;
	}

	user_dev.id.bustype = BUS_VIRTUAL;	//TODO Should we use BUS_VIRTUAL instead of BUS_USB?
	//user_dev.id.vendor = 0x06a3;
	//user_dev.id.product = 0x0764;
	user_dev.id.version = 1;
	int r = suinput_create(fd, &user_dev);
	if(r)
	{
		std::cout <<"ERROR " << r << ": Failed creating virtual device " << deviceid << ".\n";
		suinput_destroy(fd);
		fd = -1;
		return;
	}//if

	std::cout << "Successfully created virtual device " << deviceid << ".\n";
	static_deviceid++;
}


CVirtualJoy::~CVirtualJoy()
{
	suinput_destroy(fd);
}

bool CVirtualJoy::isOpen()
{
	return fd>=0;
}

int CVirtualJoy::getDeviceid()
{
	return deviceid;
}

int CVirtualJoy::get_button_status(int type)
{
	return (buttonFlags & (1ul << type) ? true : false);
}


void CVirtualJoy::send_button_event(int type, int value)
{
	uint64_t check = (value) ? (get_button_flags() | 1ul << type) : (get_button_flags() & ~(1uL << type));	//Set new button flags state
	if(check==get_button_flags()) return;	//Button already set, we do not need to emit the data again
	set_button_flags(check);				//Updating button flags
	
	suinput_emit(fd, EV_KEY, buttons_ref::BUTTONS[type], value);
	suinput_syn(fd);
}

void CVirtualJoy::send_axis_event(int type, int value)
{
	set_axis_data(type, value);
	suinput_emit(fd, EV_ABS, buttons_ref::AXES[type], value);
	suinput_syn(fd);
}


