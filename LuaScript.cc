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

#include "LuaScript.h"
#include "lua_key_codes.h"

#include <errno.h>
#include <cstring>

LuaScript::LuaScript(const std::string& filename) {
  L = luaL_newstate();
  luaL_openlibs(L);
  LUA_GLOBAL_KEYS::init_global_keys(L);

  if(luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0))
    {
      std::cout<<"Error: failed to load ("<<filename<<")"<<std::endl;
      const char* s = strerror(errno);
      //std::cout << s << std::endl;
      std::cout << lua_tostring(L, -1) << std::endl << s << std::endl;
      L = 0;
      return;
    }
}

LuaScript::~LuaScript()
{
  if(L) lua_close(L);
}

bool LuaScript::doFunctionExist(const std::string& functionName)
{
  lua_getglobal(L, functionName.c_str());
  return lua_isfunction(L, -1);
}

void LuaScript::printError(const std::string& variableName, const std::string& reason) {
  std::cout<<"Error: can't get ["<<variableName<<"]. "<<reason<<std::endl;
}


/*
std::vector<int> LuaScript::getIntVector(const std::string& name) {
  std::vector<int> v;
  lua_gettostack(name.c_str());
  if(lua_isnil(L, -1)) { // array is not found
    return std::vector<int>();
  }
  lua_pushnil(L);
  while(lua_next(L, -2)) {
    v.push_back((int)lua_tonumber(L, -1));
    lua_pop(L, 1);
  }
  clean();
  return v;
}

std::vector<std::string> LuaScript::getStringVector(const std::string& name) {
  std::vector<std::string> v;
  lua_gettostack(name.c_str());
  if(lua_isnil(L, -1)) { // array is not found
    return std::vector<std::string>();
  }
  lua_pushnil(L);
  while(lua_next(L, -2)) {
    v.push_back((std::string)lua_tostring(L, -1));
    lua_pop(L, 1);
  }
  clean();
  return v;
}

std::vector<std::string> LuaScript::getTableKeys(const std::string& name)
{
  std::vector<std::string> keys;
  lua_getglobal(L, name.c_str());
  if (!lua_istable(L, -1))
  {
    lua_pop(L, 1);
    return keys;
  }

  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    const char *key = lua_tostring(L, -2);
    if(key) keys.push_back(key);
    lua_pop(L, 1);
  }
  lua_pop(L, 1);
  
  return keys;
}
*/


void LuaScript::call_device_function(const std::string& str_func, int event_type, int event_code, int event_value)
{
  lua_getglobal(L, str_func.c_str());
  lua_pushnumber(L, event_type);
  lua_pushnumber(L, event_code);
  lua_pushnumber(L, event_value);
  lua_pcall(L, 3, 0, 0); //Lua handle, number of arguments, number of return values, error code
}

//void LuaScript::call_device_function(const device_function& df, int value)
void LuaScript::call_device_function(const std::string& str_func, int value)
{
  //lua_getglobal(L, df.lfCall.c_str());
  lua_getglobal(L, str_func.c_str());
  lua_pushnumber(L, value);
  lua_pcall(L, 1, 0, 0); //Lua handle, number of arguments, number of return values, error code
}

void LuaScript::pushcfunction(int (*_func)(lua_State*), const std::string& _name)
{
  lua_pushcfunction(L, _func);
  lua_setglobal(L, _name.c_str());
}
