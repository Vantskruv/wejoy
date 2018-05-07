#include "LuaScript.h"
#include <linux/uinput.h>    //Reference to keycodes
#include <errno.h>
#include "input_events.h"
#include <cstring>
#include "buttons_ref.h"

LuaScript::LuaScript(const std::string &filename) {
    L = luaL_newstate();
    if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
        std::cout << "Error: failed to load (" << filename << ")" << std::endl;
        const char *s = strerror(errno);
        std::cout << lua_tostring(L, -1) << std::endl << s << std::endl;
        L = 0;
        return;
    }

    if (L) luaL_openlibs(L);

    _set_global_keys();

}

LuaScript::~LuaScript() {
    if (L) lua_close(L);
}

void LuaScript::printError(const std::string &variableName, const std::string &reason) {
    std::cout << "Error: can't get [" << variableName << "]. " << reason << std::endl;
}

std::vector<std::string> LuaScript::getAllGlobals() {
    std::vector<std::string> globals;
    lua_pushglobaltable(L);
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        globals.emplace_back(lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return globals;
}

std::vector<int> LuaScript::getIntVector(const std::string &name) {
    std::vector<int> v;
    lua_gettostack(name.c_str());
    if (lua_isnil(L, -1)) { // array is not found
        return std::vector<int>();
    }
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        v.push_back((int) lua_tonumber(L, -1));
        lua_pop(L, 1);
    }
    clean();
    return v;
}

std::vector<std::string> LuaScript::getStringVector(const std::string &name) {
    std::vector<std::string> v;
    lua_gettostack(name.c_str());
    if (lua_isnil(L, -1)) { // array is not found
        return std::vector<std::string>();
    }
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        v.push_back((std::string) lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    clean();
    return v;
}


std::vector<std::string> LuaScript::getTableKeys(const std::string &name) {
    std::string code =
            "function getKeys(name) "
            "s = \"\""
            "for k, v in pairs(_G[name]) do "
            "    s = s..k..\",\" "
            "    end "
            "return s "
            "end"; // function for getting table keys
    luaL_loadstring(L,
                    code.c_str()); // execute code
    lua_pcall(L, 0, 0, 0);
    lua_getglobal(L, "getKeys"); // get function
    lua_pushstring(L, name.c_str());
    lua_pcall(L, 1, 1, 0); // execute function
    std::string test = lua_tostring(L, -1);
    std::cout << test << std::endl;
    std::vector<std::string> strings;
    std::string temp;
    for (unsigned int i = 0; i < test.size(); i++) {
        if (test.at(i) != ',') {
            temp += test.at(i);
        } else {
            strings.push_back(temp);
            temp = "";
        }
    }
    clean();
    return strings;
}


//void LuaScript::call_device_function(const device_function& df, int value)
void LuaScript::call_device_function(const std::string &str_func, int value) {
    //lua_getglobal(L, df.lfCall.c_str());
    lua_getglobal(L, str_func.c_str());
    lua_pushnumber(L, value);
    lua_pcall(L, 1, 0, 0);        //Lua handle, number of arguments, number of return values, error code
}

void LuaScript::call_value_function(std::string function, std::string name, int axis, int value) {
    lua_getglobal(L, function.c_str());
    lua_pushstring(L, name.c_str());
    lua_pushnumber(L, axis);
    lua_pushnumber(L, value);
    lua_pcall(L, 3, 0, 0);        //Lua handle, number of arguments, number of return values, error code
}

void LuaScript::call_main_function() {
    lua_getglobal(L, "main");
    lua_pcall(L, 0, 0, 0);        //Lua handle, number of arguments, number of return values, error code
}

void LuaScript::pushcfunction(int (*_func)(lua_State *), const std::string &_name) {
    lua_pushcfunction(L, _func);
    lua_setglobal(L, _name.c_str());
}

void LuaScript::_set_global_keys() {
    std::map<int, const char*>::iterator it;

    for ( it = key_map.begin(); it != key_map.end(); it++ )
    {
        lua_pushnumber(L, it->first);
        lua_setglobal(L, it->second);
    }
    lua_pushnumber(L, MAX_ABS_VAL);
    lua_setglobal(L, "MAX_ABS_VAL");
    lua_pushnumber(L, MIN_ABS_VAL);
    lua_setglobal(L, "MIN_ABS_VAL");
}

