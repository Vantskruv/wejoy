#include <unistd.h> //sleep
#include <thread>	//thread
#include <mutex>

#include "LuaScript.h"
#include "global.h"
#include "CKeyboard.h"

bool bPoll = true;
std::mutex mtx;

void updateThreadJoysticks(LuaScript& lScript)
{
	//Sleep one second to give the X11 system time to adapt.
	sleep(1);

	JoystickEvent event;
	while(bPoll)
    {
        usleep(1000);
        for(unsigned int i=0; i<GLOBAL::joyList.size(); i++)
        {
            if(GLOBAL::joyList[i]->readJoy(&event))
            {
                mtx.lock();
                if(event.isButton()) lScript.call_device_function("d" + std::to_string(i) + "_b" + std::to_string(event.number) + "_event", event.value);
                else if(event.isAxis()) lScript.call_device_function("d" + std::to_string(i) + "_a" + std::to_string(event.number) + "_event", event.value);
                mtx.unlock();
            }//if

        }//for
    }//while
}

void updateThreadKeyboard(LuaScript& lScript)
{
	sleep(1);

    CKeyboardEvent kbdEvent;
	while(bPoll)
    {
       usleep(1000);
       for(unsigned int i=0; i<GLOBAL::kbdList.size(); i++)
       {
            if(GLOBAL::kbdList[i]->readEvent(&kbdEvent))
            {
                mtx.lock();
                if(kbdEvent.isPressed) lScript.call_device_function("kbd" + std::to_string(i) + "_pressed", kbdEvent.code);
                else lScript.call_device_function("kbd" + std::to_string(i) + "_released", kbdEvent.code);
                mtx.unlock();
            }//if
       }//for
    }//while
}

//Called from user via lua script
int l_send_keyboard_event(lua_State* L)
{
	int type = lua_tonumber(L, 1);
	int value = lua_tonumber(L, 2);
	GLOBAL::vKeyboard->send_key_event(type, value);
	return 0;
}

//Called from user via lua script
int l_get_joy_button_status(lua_State* L)
{
	unsigned int id = lua_tonumber(L, 1);
	int type = lua_tonumber(L, 2);
	if(id>=GLOBAL::joyList.size())
	{
		std::cout << "ERROR get_joy_button_status: Device " << id << " does not exist.\n";
		lua_pushnumber(L, -1);
		return 1;
	}
	int status = GLOBAL::joyList[id]->get_button_status(type);
	lua_pushnumber(L, status);
	return 1;	
}


//Called from user via lua script
int l_get_joy_axis_status(lua_State* L)
{
	unsigned int id = lua_tonumber(L, 1);
	int type = lua_tonumber(L, 2);
	if(id>=GLOBAL::joyList.size())
	{
		std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
		lua_pushnumber(L, -1);
		return 1;
	}
	int status = GLOBAL::joyList[id]->get_axis_status(type);
	lua_pushnumber(L, status);
	return 1;	
}

//Called from user via lua script
int l_send_vjoy_button_event(lua_State* _L)
{
	unsigned int id = lua_tonumber(_L, 1);
	int type = lua_tonumber(_L, 2);
	int value = lua_tonumber(_L, 3);

	if(id >= GLOBAL::vJoyList.size())
	{
		std::cout << "ERROR send_vjoy_button_event: Virtual device " << id << " does not exist.\n";
		return 0;	
	}//if
	GLOBAL::vJoyList[id]->send_button_event(type, value);
	return 0;
}

//Called from user via lua script
int l_send_vjoy_axis_event(lua_State* _L)
{
	unsigned int id = lua_tonumber(_L, 1);
	int type = lua_tonumber(_L, 2);
	int value = lua_tonumber(_L, 3);

	if(id >= GLOBAL::vJoyList.size())
	{
		std::cout << "ERROR send_vjoy_axis_event: Virtual device " << id << " does not exist.\n";
		return 0;	
	}//if
	GLOBAL::vJoyList[id]->send_axis_event(type, value);
	
	return 0;
}

//Called from user via lua script
int l_get_vjoy_button_status(lua_State* L)
{
	unsigned int id = lua_tonumber(L, 1);
	int type = lua_tonumber(L, 2);
	if(id>=GLOBAL::vJoyList.size())
	{
		std::cout << "ERROR get_vjoy_button_status: Virtual device " << id << " does not exist.\n";
		lua_pushnumber(L, -1);
		return 1;
	}
	int status = GLOBAL::vJoyList[id]->get_button_status(type);
	lua_pushnumber(L, status);
	return 1;	
}


//Called from user via lua script
int l_get_vjoy_axis_status(lua_State* L)
{
	unsigned int id = lua_tonumber(L, 1);
	int type = lua_tonumber(L, 2);
	if(id>=GLOBAL::vJoyList.size())
	{
		std::cout << "ERROR get_vjoy_axis_status: Virtual device " << id << " does not exist.\n";
		lua_pushnumber(L, -1);
		return 1;
	}
	int status = GLOBAL::vJoyList[id]->get_axis_status(type);
	lua_pushnumber(L, status);
	return 1;	
}


//Populate a list of physical devices defined in user lua file
bool populate_devices(LuaScript& lScript)
{
	//Get the data from the user lua file	
	std::vector<std::array<int, 2>> dList;
	std::array<int, 2> val;
	int cIndex = 0;
	while(1)
	{
		bool noerr;
		val[0] = lScript.get<int>("devices.d" + std::to_string(cIndex) + ".vendorid", noerr);
		if(!noerr) break;
		val[1] = lScript.get<int>("devices.d" + std::to_string(cIndex) + ".productid", noerr);
		if(!noerr) break;

		dList.push_back(val);
		
		cIndex++;
	}//for


	//Populate the list of found joysticks
	for(unsigned int i=0; i<dList.size(); i++)
	{
        Joystick* cJoy = new Joystick(dList[i][0], dList[i][1], GLOBAL::joyList);
		if(!cJoy->isFound())
		{
			std::cout << "WARNING: Joystick " << std::hex << dList[i][0] << ":" << std::hex << dList[i][1] << " is not found.\n";
			delete cJoy;
			return false;
		}

		GLOBAL::joyList.push_back(cJoy);
	}//for


    //Initilize the keyboards for input
    cIndex = 0;
    while(1)
    {
        bool noerr;
        std::string kbdEventPath = lScript.get<std::string>("devices.kbd" + std::to_string(cIndex), noerr);
        if(!noerr) break;
        CKeyboard* nKBG = new CKeyboard(kbdEventPath);
        GLOBAL::kbdList.push_back(nKBG);
        cIndex++;
    }//while

	return true;
}

//Populate a alist of virtual devices defined in user lua file
bool populate_virtual_devices(LuaScript& lScript)
{
	std::vector<std::array<int, 2>> dList;
	std::array<int, 2> val;
	int cIndex = 0;
	while(1)
	{
		bool noerr;
		
		val[0] = lScript.get<int>("v_devices.v" + std::to_string(cIndex) + ".buttons", noerr);
		if(!noerr) break;
		val[1] = lScript.get<int>("v_devices.v" + std::to_string(cIndex) + ".axes", noerr);
		if(!noerr) break;

		dList.push_back(val);
		
		cIndex++;
	}//for

	//Create and populate the list of user defined virtual devices
	for(unsigned int i=0; i<dList.size(); i++)
	{
		CVirtualJoy* vJoy = new CVirtualJoy(dList[i][0], dList[i][1]);
		if (!vJoy->isOpen()) return false;
		GLOBAL::vJoyList.push_back(vJoy);
	}//for

    GLOBAL::vKeyboard = new CVirtualKeyboard();

	return true;
}

//Initialize lua functions
void link_lua_functions(LuaScript& lScript)
{
	lScript.pushcfunction(l_send_vjoy_button_event, "send_button_event");
	lScript.pushcfunction(l_send_vjoy_axis_event, "send_axis_event");
	lScript.pushcfunction(l_send_keyboard_event, "send_keyboard_event");
	lScript.pushcfunction(l_get_joy_button_status, "get_button_status");
	lScript.pushcfunction(l_get_joy_axis_status, "get_axis_status");
	lScript.pushcfunction(l_get_vjoy_button_status, "get_vjoy_button_status");
	lScript.pushcfunction(l_get_vjoy_axis_status, "get_vjoy_axis_status");
}




int main(int argc, char** argv)
{
	//TODO I need to search for information on which buttons and axes is required to correctly be found in applications, as sometimes i.e. axes are found in system, but not by applications.

    std::cout << "WeJoy v0.2 by Johannes Bergmark\n";

	if(argc<2)
	{
		std::cout << "Please type the path of your lua file.\n";
		std::cout << "I.e. 'wejoy config.lua'\n";
		return 0;
	}


	//Open the user lua file.
	LuaScript lScript(argv[1]);
	if(!lScript.isOpen()) return 0;

	if (!populate_devices(lScript)) exit(0);
	if (!populate_virtual_devices(lScript)) exit(0);
	link_lua_functions(lScript);


	std::cout << "Press 'q' and then 'ENTER' to quit!\n";	

	std::thread threadUpdateJoysticks(updateThreadJoysticks, std::ref(lScript));
    std::thread threadUpdateKeyboard(updateThreadKeyboard, std::ref(lScript));
	
	while(getchar()!='q');
	bPoll = false,
	threadUpdateJoysticks.join();
    threadUpdateKeyboard.join();
	sleep(1);

    for(unsigned int i=0; i<GLOBAL::kbdList.size(); i++) delete GLOBAL::kbdList[i];
    delete GLOBAL::vKeyboard;

	return 0;
}

