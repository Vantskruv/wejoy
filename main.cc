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

#include <array>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <linux/input.h>
#include <memory>
#include <string>
#include <unistd.h> //sleep
#include <thread>
#include <mutex>
#include <csignal>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <unordered_map>

#include "LuaScript.h"
#include "global.h"
#include "joystick.h"


void updateThreadLuaSystemLoop(std::unique_ptr<LuaScript> lScript)
{
  const auto period = std::chrono::milliseconds{1};

  //Sleep one second to give the system time to adapt.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  auto time_last = std::chrono::steady_clock::now();
  while(GLOBAL::bPoll.load(std::memory_order_acquire))
  {
    auto time_now = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_last).count();
    lScript->call_device_function("system_thread", delta);
    time_last = time_now;
    std::this_thread::sleep_until(time_last + period);
  }//while
}

void updateThreadJoystick(std::unique_ptr<LuaScript> lScript)
{
  JoystickEvent event;
  const auto period = std::chrono::milliseconds{1};

  //Sleep one second to give the system time to adapt.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  while(GLOBAL::bPoll.load(std::memory_order_acquire))
  {
    for(auto& cjoy : GLOBAL::joyList)
    {
      if(cjoy->read_joy(&event)) lScript->call_device_function(cjoy->get_description(), event.type, event.code, event.value);
    }
    
    std::this_thread::sleep_for(period);
  }//while
}

/*
//Called from user via lua script
int l_send_vkeyboard_key_event(lua_State* L)
{
  int type = lua_tonumber(L, 1);
  int value = lua_tonumber(L, 2);
  GLOBAL::vKeyboard->send_key_event(type, value);
  return 0;
}
*/

//Called from user via lua script
int l_get_joy_button_status(lua_State* L)
{
  unsigned int device_index = lua_tonumber(L, 1);
  int code = lua_tonumber(L, 2);

  if(device_index >= GLOBAL::joyList.size())
  {
    printf("ERROR get_button_status: Device with index %d do not exist.\n", device_index);
    return 0;
  }
  int status = GLOBAL::joyList[device_index]->get_button_status(code);

  lua_pushnumber(L, status);
  return 1;
}

//Called from user via lua script
int l_get_joy_rel_status(lua_State* L)
{
  unsigned int device_index = lua_tonumber(L, 1);
  int code = lua_tonumber(L, 2);
  
  if(device_index >= GLOBAL::joyList.size())
  {
    printf("ERROR get_joy_rel_status: Device with index %d do not exist.\n", device_index);
    return 0;
  }
  int status = GLOBAL::joyList[device_index]->get_rel_status(code);

  lua_pushnumber(L, status);
  return 1;
}


//Called from user via lua script
int l_get_joy_abs_status(lua_State* L)
{
  unsigned int device_index = lua_tonumber(L, 1);
  int code = lua_tonumber(L, 2);

  if(device_index >= GLOBAL::joyList.size())
  {
    printf("ERROR get_joy_abs_status: Device with index %d do not exist.\n", device_index);
    return 0;
  }
  int status = GLOBAL::joyList[device_index]->get_abs_status(code);

  lua_pushnumber(L, status);
  return 1;
}

//Called from user via lua script
int l_send_vjoy_button_event(lua_State* _L)
{
  unsigned int id = lua_tonumber(_L, 1);
  int code = lua_tonumber(_L, 2);
  int value = lua_tonumber(_L, 3);

  if(id >= GLOBAL::vJoyList.size())
  {
    std::cout << "ERROR send_vjoy_button_event: Virtual device " << id << " does not exist.\n";
    return 0;	
  }//if
  
  GLOBAL::vJoyList[id]->send_button_event(code, value);
  return 0;
}

//Called from user via lua script
int l_send_vjoy_abs_event(lua_State* _L)
{
  unsigned int id = lua_tonumber(_L, 1);
  int type = lua_tonumber(_L, 2);
  int value = lua_tonumber(_L, 3);

  if(id >= GLOBAL::vJoyList.size())
  {
    std::cout << "ERROR send_vjoy_abs_event: Virtual device " << id << " does not exist.\n";
    return 0;	
  }//if
  GLOBAL::vJoyList[id]->send_abs_event(type, value);
  
  return 0;
}

//Called from user via lua script
int l_send_vjoy_rel_event(lua_State* _L)
{
  unsigned int id = lua_tonumber(_L, 1);
  int type = lua_tonumber(_L, 2);
  int value = lua_tonumber(_L, 3);

  if(id >= GLOBAL::vJoyList.size())
  {
    std::cout << "ERROR send_vjoy_rel_event: Virtual device " << id << " does not exist.\n";
    return 0;	
  }//if
  GLOBAL::vJoyList[id]->send_rel_event(type, value);
  
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
int l_get_vjoy_abs_status(lua_State* L)
{
  unsigned int id = lua_tonumber(L, 1);
  int type = lua_tonumber(L, 2);
  if(id>=GLOBAL::vJoyList.size())
  {
    std::cout << "ERROR get_vjoy_abs_status: Virtual device " << id << " does not exist.\n";
    lua_pushnumber(L, -1);
    return 1;
  }
  int status = GLOBAL::vJoyList[id]->get_abs_status(type);
  lua_pushnumber(L, status);
  return 1;	
}

//Called from user via lua script
int l_get_vjoy_rel_status(lua_State* L)
{
  unsigned int id = lua_tonumber(L, 1);
  int type = lua_tonumber(L, 2);
  if(id>=GLOBAL::vJoyList.size())
  {
    std::cout << "ERROR get_vjoy_rel_status: Virtual device " << id << " does not exist.\n";
    lua_pushnumber(L, -1);
    return 1;
  }
  int status = GLOBAL::vJoyList[id]->get_rel_status(type);
  lua_pushnumber(L, status);
  return 1;	
}


int l_get_global_value(lua_State* L)
{
  if (!lua_isstring(L, 1))
  {
    std::cout << "ERROR: In get_global_value, argument 1 must be a string.\n";
    lua_pushnumber(L, 0.f);
    return 1;
  }

  const char* id = lua_tostring(L, 1);

  auto it = GLOBAL::lua_global_values.find(id);
  if (it == GLOBAL::lua_global_values.end())
  {
    std::cout << "ERROR: In get_global_value, global value " << id << " does not exist.\n";
    lua_pushnumber(L, 0.f);
    return 1;
  }

  lua_pushnumber(L, it->second);
  return 1;
}

int l_set_global_value(lua_State* L)
{
  if (!lua_isstring(L, 1))
  {
    std::cout << "ERROR: In set_global_value, argument 1 must be a string.\n";
    return luaL_error(L, "ERROR: In set_global_value, argument 1 must be a string.");
  }
  if (!lua_isnumber(L, 2))
  {
    std:: cout << "ERROR: In set_global_value, argument 2 must be a number.\n";
    return luaL_error(L, "ERROR: In set_global_value, argument 2 must be a number.");
  }

  const char* id = lua_tostring(L, 1);
  const float value = lua_tonumber(L, 2);

  auto it = GLOBAL::lua_global_values.find(id);
  if (it == GLOBAL::lua_global_values.end())
  {
    std::cout << "ERROR: In set_global_value, global value " << id << " does not exist.\n";
    return luaL_error(L, ("ERROR: In set_global_value, global value " + std::string(id) + " does not exist").c_str());
  }

  it->second = value;

  return 0;
}

bool populate_global_values(LuaScript& lScript)
{
  printf("\nList of added global variables\n");

  lua_State*L = lScript.get_lua_state();

  lua_getglobal(L, "global_values");
  if(!lua_istable(L, -1)) return true;

  lua_pushnil(L);
  while(lua_next(L, -2)!=0)
  {
    const char* variable_name = lua_tostring(L, -2);
    if(!variable_name || !lua_isnumber(L, -1))
    {
      std::cerr << "ERROR: Wrong format in global_values table.\n";
      return false;
    }

    float variable_value = lua_tonumber(L, -1);

    GLOBAL::lua_global_values.emplace(variable_name, variable_value);
    printf("%s\n", variable_name);

    lua_pop(L, 1);
  }
  lua_pop(L, 1);

  return true;
}

//Populate a list of physical devices defined in user lua file
bool populate_devices(LuaScript& lScript)
{
  printf("\nList of added physical devices\n");

  std::vector<std::string> device_paths;

  lua_State* L = lScript.get_lua_state();

  lua_getglobal(L, "devices");
  if(!lua_istable(L, -1)) return true;
  lua_pushnil(L);
  while(lua_next(L, -2))
  {
    if(!lua_isstring(L, -1))
    {
      std::cerr << "ERROR: Invalid format in table devices.\n";
      return false;
    }
    device_paths.push_back(lua_tostring(L, -1));
    lua_pop(L, 1); // Pop next
  }

  for(unsigned int idx = 0; idx<device_paths.size(); idx++)
  {
    std::unique_ptr<Joystick> cjoy = std::make_unique<Joystick>(device_paths[idx]);
    if(!cjoy->is_open())
    {
      std::cerr << "ERROR: Failed opening device:\n" << device_paths[idx] << std::endl;
      GLOBAL::joyList.clear();
      return false;
    }
    cjoy->set_description("d" + std::to_string(idx) + "_event");
    std::cout << cjoy->get_device_name() << std::endl;
    GLOBAL::joyList.push_back(std::move(cjoy));
  }

  return true;
}


//Populate a alist of virtual devices defined in user lua file
bool populate_virtual_devices(LuaScript& lScript, bool check = false)
{
  printf("\nList of added virtual devices\n");

  lua_State* L = lScript.get_lua_state();

  lua_getglobal(L, "virtualjoys");
  if(!lua_istable(L, -1)) 
  {
    return true;
  }

  int virtualjoys_index = lua_gettop(L);
  lua_pushnil(L);
  while(lua_next(L, virtualjoys_index) != 0)
  {
    if(!lua_istable(L, -1)) 
    {
      lua_pop(L, 1);
      continue;
    }

    lua_getfield(L, -1, "name");
    std::string name = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
    lua_pop(L, 1); // Pop name

    lua_getfield(L, -1, "productid");
    uint16_t vendorid = lua_isnumber(L, -1) ? lua_tointeger(L, -1) : 0xFED1;
    lua_pop(L, 1); // Pop vendorid

    lua_getfield(L, -1, "vendorid");
    uint16_t productid = lua_isstring(L, -1) ? lua_tointeger(L, -1) : 0x001;
    lua_pop(L, 1); // Pop productid

    
    lua_getfield(L, -1, "buttons");
    std::vector<uint16_t> buttons = LuaScript::read_int_vector<uint16_t>(L, lua_gettop(L));
    lua_pop(L, 1); // Pop buttons

    lua_getfield(L, -1, "rels");
    std::vector<uint16_t> rels = LuaScript::read_int_vector<uint16_t>(L, lua_gettop(L));
    lua_pop(L, 1); // Pop rels
    
    lua_getfield(L, -1, "abs");
    std::vector<CVirtualJoy::ABSData> labsdata;
    if(lua_istable(L, -1))
    {
      int idx = lua_gettop(L);
      lua_pushnil(L);
      while(lua_next(L, idx)!=0)
      {
        if(!lua_istable(L, -1))
        {
          lua_pop(L, 1);
          continue;
        }
        CVirtualJoy::ABSData absdata;
        lua_getfield(L, -1, "code");
        if(lua_isnumber(L, -1))
        {
          absdata.code = static_cast<uint32_t>(lua_tointeger(L, -1));
        }
        else
        {
          printf("Wrong format of abs axis code in virtual device table\n");
          absdata.code = 0;
        }
        lua_pop(L, 1); // Pop code
        lua_getfield(L, -1, "min");
        absdata.absinfo.minimum = lua_isnumber(L, -1) ? static_cast<uint32_t>(lua_tointeger(L, -1)) : -32767;
        lua_pop(L, 1); // Pop min
        lua_getfield(L, -1, "max");
        absdata.absinfo.maximum = lua_isnumber(L, -1) ? static_cast<uint32_t>(lua_tointeger(L, -1)) : 32767;
        lua_pop(L, 1); // Pop max
        lua_getfield(L, -1, "fuzz");
        absdata.absinfo.fuzz = lua_isnumber(L, -1) ? static_cast<uint32_t>(lua_tointeger(L, -1)) : 0;
        lua_pop(L, 1); // Pop fuzz
        lua_getfield(L, -1, "flat");
        absdata.absinfo.flat = lua_isnumber(L, -1) ? static_cast<uint32_t>(lua_tointeger(L, -1)) : 0;
        lua_pop(L, 1); // Pop flat

        lua_pop(L, 1); // Pop abs element
        
        labsdata.push_back(absdata);
      }
    }
    lua_pop(L, 1); // Pop abs table

    std::unique_ptr<CVirtualJoy> vJoy = std::make_unique<CVirtualJoy>(buttons, labsdata, rels, name, vendorid, productid);
    if(!vJoy->is_open()) return false;
    if(check)
    {
      vJoy->print_verbose_info();
      std::cout << std::endl;
    }
    else std::cout << vJoy->get_name() << std::endl;
    GLOBAL::vJoyList.push_back(std::move(vJoy));

    lua_pop(L, 1); // Pop found virtual device table
  }

  //printf("Virtual keyboard added\n");
  //GLOBAL::vKeyboard = std::make_unique<CVirtualKeyboard>();

  return true;
}

//Initialize lua functions
void link_lua_functions(LuaScript& lScript)
{
  lScript.pushcfunction(l_get_global_value, "get_global_value");
  lScript.pushcfunction(l_set_global_value, "set_global_value");

  //lScript.pushcfunction(l_send_vkeyboard_key_event, "send_vkeyboard_key_event");

  lScript.pushcfunction(l_send_vjoy_button_event, "send_vjoy_button_event");
  lScript.pushcfunction(l_send_vjoy_abs_event, "send_vjoy_abs_event");
  lScript.pushcfunction(l_send_vjoy_rel_event, "send_vjoy_rel_event");
  lScript.pushcfunction(l_get_vjoy_button_status, "get_vjoy_button_status");
  lScript.pushcfunction(l_get_vjoy_abs_status, "get_vjoy_abs_status");
  lScript.pushcfunction(l_get_vjoy_rel_status, "get_vjoy_rel_status");

  lScript.pushcfunction(l_get_joy_button_status, "get_joy_button_status");
  lScript.pushcfunction(l_get_joy_abs_status, "get_joy_abs_status");
  lScript.pushcfunction(l_get_joy_rel_status, "get_joy_rel_status");
}



void cleanup()
{
  std::cout << "Cleaning up memory ...\n";
  GLOBAL::joyList.clear();
  GLOBAL::vJoyList.clear();
//	GLOBAL::vKeyboard.reset();
}

void print_help()
{
  std::cout << "Please type the path of your lua file.\n"
            << "I.e. 'wejoy config.lua'\n"
            << "Options for printing device information:\n"
            << "--list-devices                                  List available devices in the /dev/input/by-id/\n"
            << "--device-info [event path]                      Show specific device info\n"
            << "--virtual-device-info [config.lua]							Show if your virtual devices is correctly loaded\n";
}

void read_args(char argc, char** argv, std::string& _path)
{
  _path = "";

  if(argc<2)
  {
    print_help();
    return;
  }

  if(argc == 2)
  {
    if(std::strcmp(argv[1], "--list-devices") == 0)
    {
      std::vector<std::string> available_devices = Joystick::SystemDeviceData::get_device_paths();
      printf("Devices found %lu\n", available_devices.size());
      for(const auto& device_path : available_devices) printf("%s\n", device_path.c_str());
      return;
    }
    _path = argv[1];
  }
  else if(argc == 3 && std::strcmp(argv[1], "--device-info") == 0)
  {
    Joystick iJoy(argv[2]);
    iJoy.print_verbose_info();
    std::cout << std::endl;
  }
  else if(argc == 3 && std::strcmp(argv[1], "--virtual-device-info") == 0)
  {
    LuaScript lscript_check(argv[2]);
    populate_virtual_devices(lscript_check, true);
  }
  else print_help();
}

std::atomic<bool> exit_signal(false);
std::condition_variable cv_signal;

void exit_handler(int signum)
{
  exit_signal = true;
  cv_signal.notify_one();
}

int main(int argc, char** argv)
{
  // Read arguments
  std::string path;
  read_args(argc, argv, path);
  if(path.length() == 0) return 0;

  // Print version and my glorious name
  std::cout << "WeJoy v0.3 by Johannes Bergmark\n\n";

  // Setup signals for termination
  signal(SIGTERM, exit_handler);
  signal(SIGINT, exit_handler);
  signal(SIGHUP, exit_handler);

  // We open the luascript for the first time to read the number of physical and virtual devices
  LuaScript* lscript_init = new LuaScript(path);
  if(!lscript_init->isOpen() || !populate_devices(*lscript_init) || !populate_virtual_devices(*lscript_init) || !populate_global_values(*lscript_init))
  {
    cleanup();
    std::cout << "WeJoy failed to start because of errors.\n";
    return -1;
  }
  delete lscript_init;

  // Create and start thread for polling physical devices.
  std::unique_ptr<LuaScript> luascript_joystic_poll_thread = std::make_unique<LuaScript>(argv[1]);
  link_lua_functions(*luascript_joystic_poll_thread);
  std::thread physical_joysticks_thread(updateThreadJoystick, std::move(luascript_joystic_poll_thread));

  // Create and start thread for the system_thread feature in the lua script
  std::unique_ptr<LuaScript> luascript_system_thread = std::make_unique<LuaScript>(argv[1]);
  link_lua_functions(*luascript_system_thread);
  std::thread lua_system_thread(updateThreadLuaSystemLoop, std::move(luascript_system_thread));

  // Wait for termination signal
  std::cout << "Press 'Ctrl C' to quit.\n";	
  std::mutex mutex_signal;
  std::unique_lock<std::mutex> lock(mutex_signal);
  cv_signal.wait(lock, [&] { return exit_signal.load();});
  GLOBAL::bPoll.store(false, std::memory_order_release);

  std::cout << "\nClosing threads ...\n";
  lua_system_thread.join();
  physical_joysticks_thread.join();

  sleep(1);
  
  cleanup();

  std::cout << "See you later, goodbye!\n";
  return 1;
}
