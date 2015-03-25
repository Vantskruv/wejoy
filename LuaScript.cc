#include "LuaScript.h"
#include <linux/uinput.h>	//Reference to keycodes
#include <errno.h>
#include <cstring>

LuaScript::LuaScript(const std::string& filename) {
    L = luaL_newstate();
    if(luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0))
	{
        std::cout<<"Error: failed to load ("<<filename<<")"<<std::endl;
		const char* s = strerror(errno);
		//std::cout << s << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl << s << std::endl;
		L = 0;
		return;
    }

	if(L) luaL_openlibs(L);

	_set_global_keys();

}

LuaScript::~LuaScript() {
	if(L) lua_close(L);
}

void LuaScript::printError(const std::string& variableName, const std::string& reason) {
	std::cout<<"Error: can't get ["<<variableName<<"]. "<<reason<<std::endl;
}

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


std::vector<std::string> LuaScript::getTableKeys(const std::string& name) {
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
    lua_pcall(L,0,0,0);
    lua_getglobal(L, "getKeys"); // get function
    lua_pushstring(L, name.c_str());
    lua_pcall(L, 1 , 1, 0); // execute function
    std::string test = lua_tostring(L, -1);
    std::vector<std::string> strings;
    std::string temp = "";
    std::cout<<"TEMP:"<<test<<std::endl;
    for(unsigned int i = 0; i < test.size(); i++) {     
        if(test.at(i) != ',') {
            temp += test.at(i);
        } else {
            strings.push_back(temp);
            temp= "";
        }
    }
    clean();
    return strings;
}


//void LuaScript::call_device_function(const device_function& df, int value)
void LuaScript::call_device_function(const std::string& str_func, int value)
{
	//lua_getglobal(L, df.lfCall.c_str());
	lua_getglobal(L, str_func.c_str());
	lua_pushnumber(L, value);
	lua_pcall(L, 1, 0, 0);		//Lua handle, number of arguments, number of return values, error code
}

void LuaScript::pushcfunction(int (*_func)(lua_State*), const std::string& _name)
{
	lua_pushcfunction(L, _func);
	lua_setglobal(L, _name.c_str());
}

void LuaScript::_set_global_keys()
{
	lua_pushnumber(L, KEY_ESC);
	lua_setglobal(L, "KEY_ESC");
	lua_pushnumber(L, KEY_1);
	lua_setglobal(L, "KEY_1");
	lua_pushnumber(L, KEY_2);
	lua_setglobal(L, "KEY_2");
	lua_pushnumber(L, KEY_3);
	lua_setglobal(L, "KEY_3");
	lua_pushnumber(L, KEY_4);
	lua_setglobal(L, "KEY_4");
	lua_pushnumber(L, KEY_5);
	lua_setglobal(L, "KEY_5");
	lua_pushnumber(L, KEY_6);
	lua_setglobal(L, "KEY_6");
	lua_pushnumber(L, KEY_7);
	lua_setglobal(L, "KEY_7");
	lua_pushnumber(L, KEY_8);
	lua_setglobal(L, "KEY_8");
	lua_pushnumber(L, KEY_9);
	lua_setglobal(L, "KEY_9");
	lua_pushnumber(L, KEY_0);
	lua_setglobal(L, "KEY_0");
	lua_pushnumber(L, KEY_MINUS);
	lua_setglobal(L, "KEY_MINUS");
	lua_pushnumber(L, KEY_EQUAL);
	lua_setglobal(L, "KEY_EQUAL");
	lua_pushnumber(L, KEY_BACKSPACE);
	lua_setglobal(L, "KEY_BACKSPACE");
	lua_pushnumber(L, KEY_TAB);
	lua_setglobal(L, "KEY_TAB");
	lua_pushnumber(L, KEY_Q);
	lua_setglobal(L, "KEY_Q");
	lua_pushnumber(L, KEY_W);
	lua_setglobal(L, "KEY_W");
	lua_pushnumber(L, KEY_E);
	lua_setglobal(L, "KEY_E");
	lua_pushnumber(L, KEY_R);
	lua_setglobal(L, "KEY_R");
	lua_pushnumber(L, KEY_T);
	lua_setglobal(L, "KEY_T");
	lua_pushnumber(L, KEY_Y);
	lua_setglobal(L, "KEY_Y");
	lua_pushnumber(L, KEY_U);
	lua_setglobal(L, "KEY_U");
	lua_pushnumber(L, KEY_I);
	lua_setglobal(L, "KEY_I");
	lua_pushnumber(L, KEY_O);
	lua_setglobal(L, "KEY_O");
	lua_pushnumber(L, KEY_P);
	lua_setglobal(L, "KEY_P");
	lua_pushnumber(L, KEY_LEFTBRACE);
	lua_setglobal(L, "KEY_LEFTBRACE");
	lua_pushnumber(L, KEY_RIGHTBRACE);
	lua_setglobal(L, "KEY_RIGHTBRACE");
	lua_pushnumber(L, KEY_ENTER);
	lua_setglobal(L, "KEY_ENTER");
	lua_pushnumber(L, KEY_LEFTCTRL);
	lua_setglobal(L, "KEY_LEFTCTRL");
	lua_pushnumber(L, KEY_A);
	lua_setglobal(L, "KEY_A");
	lua_pushnumber(L, KEY_S);
	lua_setglobal(L, "KEY_S");
	lua_pushnumber(L, KEY_D);
	lua_setglobal(L, "KEY_D");
	lua_pushnumber(L, KEY_F);
	lua_setglobal(L, "KEY_F");
	lua_pushnumber(L, KEY_G);
	lua_setglobal(L, "KEY_G");
	lua_pushnumber(L, KEY_H);
	lua_setglobal(L, "KEY_H");
	lua_pushnumber(L, KEY_J);
	lua_setglobal(L, "KEY_J");
	lua_pushnumber(L, KEY_K);
	lua_setglobal(L, "KEY_K");
	lua_pushnumber(L, KEY_L);
	lua_setglobal(L, "KEY_L");
	lua_pushnumber(L, KEY_SEMICOLON);
	lua_setglobal(L, "KEY_SEMICOLON");
	lua_pushnumber(L, KEY_APOSTROPHE);
	lua_setglobal(L, "KEY_APOSTROPHE");
	lua_pushnumber(L, KEY_GRAVE);
	lua_setglobal(L, "KEY_GRAVE");
	lua_pushnumber(L, KEY_LEFTSHIFT);
	lua_setglobal(L, "KEY_LEFTSHIFT");
	lua_pushnumber(L, KEY_BACKSLASH);
	lua_setglobal(L, "KEY_BACKSLASH");
	lua_pushnumber(L, KEY_Z);
	lua_setglobal(L, "KEY_Z");
	lua_pushnumber(L, KEY_X);
	lua_setglobal(L, "KEY_X");
	lua_pushnumber(L, KEY_C);
	lua_setglobal(L, "KEY_C");
	lua_pushnumber(L, KEY_V);
	lua_setglobal(L, "KEY_V");
	lua_pushnumber(L, KEY_B);
	lua_setglobal(L, "KEY_B");
	lua_pushnumber(L, KEY_N);
	lua_setglobal(L, "KEY_N");
	lua_pushnumber(L, KEY_M);
	lua_setglobal(L, "KEY_M");
	lua_pushnumber(L, KEY_COMMA);
	lua_setglobal(L, "KEY_COMMA");
	lua_pushnumber(L, KEY_DOT);
	lua_setglobal(L, "KEY_DOT");
	lua_pushnumber(L, KEY_SLASH);
	lua_setglobal(L, "KEY_SLASH");
	lua_pushnumber(L, KEY_RIGHTSHIFT);
	lua_setglobal(L, "KEY_RIGHTSHIFT");
	lua_pushnumber(L, KEY_KPASTERISK);
	lua_setglobal(L, "KEY_KPASTERISK");
	lua_pushnumber(L, KEY_LEFTALT);
	lua_setglobal(L, "KEY_LEFTALT");
	lua_pushnumber(L, KEY_SPACE);
	lua_setglobal(L, "KEY_SPACE");
	lua_pushnumber(L, KEY_CAPSLOCK);
	lua_setglobal(L, "KEY_CAPSLOCK");
	lua_pushnumber(L, KEY_F1);
	lua_setglobal(L, "KEY_F1");
	lua_pushnumber(L, KEY_F2);
	lua_setglobal(L, "KEY_F2");
	lua_pushnumber(L, KEY_F3);
	lua_setglobal(L, "KEY_F3");
	lua_pushnumber(L, KEY_F4);
	lua_setglobal(L, "KEY_F4");
	lua_pushnumber(L, KEY_F5);
	lua_setglobal(L, "KEY_F5");
	lua_pushnumber(L, KEY_F6);
	lua_setglobal(L, "KEY_F6");
	lua_pushnumber(L, KEY_F7);
	lua_setglobal(L, "KEY_F7");
	lua_pushnumber(L, KEY_F8);
	lua_setglobal(L, "KEY_F8");
	lua_pushnumber(L, KEY_F9);
	lua_setglobal(L, "KEY_F9");
	lua_pushnumber(L, KEY_F10);
	lua_setglobal(L, "KEY_F10");
	lua_pushnumber(L, KEY_NUMLOCK);
	lua_setglobal(L, "KEY_NUMLOCK");
	lua_pushnumber(L, KEY_SCROLLLOCK);
	lua_setglobal(L, "KEY_SCROLLLOCK");
	lua_pushnumber(L, KEY_KP7);
	lua_setglobal(L, "KEY_KP7");
	lua_pushnumber(L, KEY_KP8);
	lua_setglobal(L, "KEY_KP8");
	lua_pushnumber(L, KEY_KP9);
	lua_setglobal(L, "KEY_KP9");
	lua_pushnumber(L, KEY_KPMINUS);
	lua_setglobal(L, "KEY_KPMINUS");
	lua_pushnumber(L, KEY_KP4);
	lua_setglobal(L, "KEY_KP4");
	lua_pushnumber(L, KEY_KP5);
	lua_setglobal(L, "KEY_KP5");
	lua_pushnumber(L, KEY_KP6);
	lua_setglobal(L, "KEY_KP6");
	lua_pushnumber(L, KEY_KPPLUS);
	lua_setglobal(L, "KEY_KPPLUS");
	lua_pushnumber(L, KEY_KP1);
	lua_setglobal(L, "KEY_KP1");
	lua_pushnumber(L, KEY_KP2);
	lua_setglobal(L, "KEY_KP2");
	lua_pushnumber(L, KEY_KP3);
	lua_setglobal(L, "KEY_KP3");
	lua_pushnumber(L, KEY_KP0);
	lua_setglobal(L, "KEY_KP0");
	lua_pushnumber(L, KEY_KPDOT);
	lua_setglobal(L, "KEY_KPDOT");
	lua_pushnumber(L, KEY_ZENKAKUHANKAKU);
	lua_setglobal(L, "KEY_ZENKAKUHANKAKU");
	lua_pushnumber(L, KEY_102ND);
	lua_setglobal(L, "KEY_102ND");
	lua_pushnumber(L, KEY_F11);
	lua_setglobal(L, "KEY_F11");
	lua_pushnumber(L, KEY_F12);
	lua_setglobal(L, "KEY_F12");
	lua_pushnumber(L, KEY_RO);
	lua_setglobal(L, "KEY_RO");
	lua_pushnumber(L, KEY_KATAKANA);
	lua_setglobal(L, "KEY_KATAKANA");
	lua_pushnumber(L, KEY_HIRAGANA);
	lua_setglobal(L, "KEY_HIRAGANA");
	lua_pushnumber(L, KEY_HENKAN);
	lua_setglobal(L, "KEY_HENKAN");
	lua_pushnumber(L, KEY_KATAKANAHIRAGANA);
	lua_setglobal(L, "KEY_KATAKANAHIRAGANA");
	lua_pushnumber(L, KEY_MUHENKAN);
	lua_setglobal(L, "KEY_MUHENKAN");
	lua_pushnumber(L, KEY_KPJPCOMMA);
	lua_setglobal(L, "KEY_KPJPCOMMA");
	lua_pushnumber(L, KEY_KPENTER);
	lua_setglobal(L, "KEY_KPENTER");
	lua_pushnumber(L, KEY_RIGHTCTRL);
	lua_setglobal(L, "KEY_RIGHTCTRL");
	lua_pushnumber(L, KEY_KPSLASH);
	lua_setglobal(L, "KEY_KPSLASH");
	lua_pushnumber(L, KEY_SYSRQ);
	lua_setglobal(L, "KEY_SYSRQ");
	lua_pushnumber(L, KEY_RIGHTALT);
	lua_setglobal(L, "KEY_RIGHTALT");
	lua_pushnumber(L, KEY_LINEFEED);
	lua_setglobal(L, "KEY_LINEFEED");
	lua_pushnumber(L, KEY_HOME);
	lua_setglobal(L, "KEY_HOME");
	lua_pushnumber(L, KEY_UP);
	lua_setglobal(L, "KEY_UP");
	lua_pushnumber(L, KEY_PAGEUP);
	lua_setglobal(L, "KEY_PAGEUP");
	lua_pushnumber(L, KEY_LEFT);
	lua_setglobal(L, "KEY_LEFT");
	lua_pushnumber(L, KEY_RIGHT);
	lua_setglobal(L, "KEY_RIGHT");
	lua_pushnumber(L, KEY_END);
	lua_setglobal(L, "KEY_END");
	lua_pushnumber(L, KEY_DOWN);
	lua_setglobal(L, "KEY_DOWN");
	lua_pushnumber(L, KEY_PAGEDOWN);
	lua_setglobal(L, "KEY_PAGEDOWN");
	lua_pushnumber(L, KEY_INSERT);
	lua_setglobal(L, "KEY_INSERT");
	lua_pushnumber(L, KEY_DELETE);
	lua_setglobal(L, "KEY_DELETE");
	lua_pushnumber(L, KEY_MACRO);
	lua_setglobal(L, "KEY_MACRO");
	lua_pushnumber(L, KEY_MUTE);
	lua_setglobal(L, "KEY_MUTE");
	lua_pushnumber(L, KEY_VOLUMEDOWN);
	lua_setglobal(L, "KEY_VOLUMEDOWN");
	lua_pushnumber(L, KEY_VOLUMEUP);
	lua_setglobal(L, "KEY_VOLUMEUP");
	lua_pushnumber(L, KEY_POWER);
	lua_setglobal(L, "KEY_POWER");
	lua_pushnumber(L, KEY_KPEQUAL);
	lua_setglobal(L, "KEY_KPEQUAL");
	lua_pushnumber(L, KEY_KPPLUSMINUS);
	lua_setglobal(L, "KEY_KPPLUSMINUS");
	lua_pushnumber(L, KEY_PAUSE);
	lua_setglobal(L, "KEY_PAUSE");
}

