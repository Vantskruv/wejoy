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

#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include <string>
#include <vector>
#include <iostream>

extern "C" {
# include <lua5.2/lua.h>
# include <lua5.2/lauxlib.h>
# include <lua5.2/lualib.h>
}


class LuaScript {
public:
  LuaScript(const std::string&);
  ~LuaScript();
  bool doFunctionExist(const std::string& functionName);
  void printError(const std::string& variableName, const std::string& reason);
  std::vector<int> getIntVector(const std::string& name);
  std::vector<std::string> getStringVector(const std::string& name);
  std::vector<std::string> getTableKeys(const std::string& name);
  //void call_device_function(const device_function&, int);
  void call_device_function(const std::string&, int);
  void pushcfunction(int (*)(lua_State*), const std::string&);
  bool isOpen(){return L;};
    
  inline void clean() {
    int n = lua_gettop(L);
    lua_pop(L, n);
  }


  template<typename T>
  T get(const std::string& variableName, bool& _noerr)
  {
    _noerr = true;
	
    if(!L) 
      {
	printError(variableName, "Script is not loaded");
	_noerr = false;
	return lua_getdefault<T>();
      }
      
    T result;
    if(lua_gettostack(variableName))
      { // variable succesfully on top of stack
	result = lua_get<T>(variableName);  
      }
    else
      {
	_noerr = false;
	result = lua_getdefault<T>();
      }

     
    clean();
    return result;
  }

  bool lua_gettostack(const std::string& variableName) {
    level = 0;
    std::string var = "";
    for(unsigned int i = 0; i < variableName.size(); i++) {
      if(variableName.at(i) == '.') {
	if(level == 0) {
	  lua_getglobal(L, var.c_str());
	} else {
	  lua_getfield(L, -1, var.c_str());
	}
            
	if(lua_isnil(L, -1)) {
	  //printError(variableName, "1:At level " + std::to_string(level) + ", " + var + " is not defined");
	  return false;
	} else {
	  var = "";
	  level++;
	}
      } else {
	var += variableName.at(i);
      }
    }
    if(level == 0) {
      lua_getglobal(L, var.c_str());
    } else {
      lua_getfield(L, -1, var.c_str());
    }
    if(lua_isnil(L, -1)) {
      //printError(variableName, "2:At level " + std::to_string(level) + ", " + var + " is not defined");
      return false;
    }

    return true;       
  }

  // Generic get
  template<typename T>
  T lua_get(const std::string& variableName) {
    return 0;
  }

  template<typename T>
  T lua_getdefault() {
    return 0;
  }
   
private:
  lua_State* L;
  int level;

  void _set_global_keys();
};

// Specializations

template <> 
inline bool LuaScript::lua_get<bool>(const std::string& variableName) {
  return (bool)lua_toboolean(L, -1);
}

template <> 
inline float LuaScript::lua_get<float>(const std::string& variableName) {
  if(!lua_isnumber(L, -1)) {
    printError(variableName, "Not a number");
  }
  return (float)lua_tonumber(L, -1);
}

template <>
inline int LuaScript::lua_get<int>(const std::string& variableName) {
  if(!lua_isnumber(L, -1)) {
    printError(variableName, "Not a number");
  }
  return (int)lua_tonumber(L, -1);
}

template <>
inline std::string LuaScript::lua_get<std::string>(const std::string& variableName) {
  std::string s = "null";
  if(lua_isstring(L, -1)) {
    s = std::string(lua_tostring(L, -1));
  } else {
    printError(variableName, "Not a string");
  }
  return s;
}

template<>
inline std::string LuaScript::lua_getdefault<std::string>() {
  return "null";
}

#endif
