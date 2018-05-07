#include <unistd.h> //sleep
#include <thread>    //thread

#include "LuaScript.h"
#include "global.h"
#include <libevdev-1.0/libevdev/libevdev.h>

bool bPoll = true;

void updateThread(LuaScript &lScript) {
    //Sleep one second to give the X11 system time to adapt.
    sleep(1);

    int rc = 1;
    while (bPoll) {
        for (Joystick *joy : GLOBAL::joyList) {
            //TODO: look into using libevdev_event_code_get_name to generate function names, to make it so that we can use names for some axis, instead of just relying on ids.
            struct input_event ev;
            rc = libevdev_next_event(joy->get_dev(), LIBEVDEV_READ_FLAG_NORMAL, &ev);
            if (rc == 0) {
                if (ev.type == EV_KEY) {
                    lScript.call_button_function(joy->getLuaName(), ev.code, ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_b" + std::to_string(ev.code) + "_event",
                                                 ev.value);

                } else if (ev.type == EV_ABS) {
                    lScript.call_axis_function(joy->getLuaName(), ev.code, ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_a" + std::to_string(ev.code) + "_event",
                                                 ev.value);
                }
            }
        }//for
    }

}

//Called from user via lua script
int l_send_keyboard_event(lua_State *L) {
    int type = lua_tonumber(L, 1);
    int value = lua_tonumber(L, 2);
    GLOBAL::vKeyboard.send_key_event(type, value);
    return 0;
}

//Called from user via lua script
int l_get_joy_button_status(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_button_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    int status = GLOBAL::joyList[id]->get_button_status(type);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_status(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    int status = GLOBAL::joyList[id]->get_axis_status(type);
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_send_vjoy_button_event(lua_State *_L) {
    unsigned int id = lua_tonumber(_L, 1);
    int type = lua_tonumber(_L, 2);
    int value = lua_tonumber(_L, 3);

    if (id >= GLOBAL::vJoyList.size()) {
        std::cout << "ERROR send_vjoy_button_event: Virtual device " << id << " does not exist.\n";
        return 0;
    }//if
    GLOBAL::vJoyList[id]->send_button_event(type, value);
    return 0;
}

//Called from user via lua script
int l_send_vjoy_axis_event(lua_State *_L) {
    unsigned int id = lua_tonumber(_L, 1);
    int type = lua_tonumber(_L, 2);
    int value = lua_tonumber(_L, 3);

    if (id >= GLOBAL::vJoyList.size()) {
        std::cout << "ERROR send_vjoy_axis_event: Virtual device " << id << " does not exist.\n";
        return 0;
    }//if
    GLOBAL::vJoyList[id]->send_axis_event(type, value);

    return 0;
}

//Called from user via lua script
int l_get_vjoy_button_status(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::vJoyList.size()) {
        std::cout << "ERROR get_vjoy_button_status: Virtual device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    int status = GLOBAL::vJoyList[id]->get_button_status(type);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_vjoy_axis_status(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::vJoyList.size()) {
        std::cout << "ERROR get_vjoy_axis_status: Virtual device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    int status = GLOBAL::vJoyList[id]->get_axis_status(type);
    lua_pushnumber(L, status);
    return 1;
}


//Populate a list of physical devices defined in user lua file
bool populate_devices(LuaScript &lScript) {
    std::vector<LuaStick> dList;
    LuaStick val;

    for (std::string s : lScript.getTableKeys("devices")) {
        val = LuaStick();
        val.lua_name = s;
        bool noerr;
        val.name = lScript.get<std::string>("devices." + s + ".name", noerr);
        if (!noerr) {
            val.product_id = lScript.get<int>("devices." + s + ".productid", noerr);;
            if (!noerr) break;
            val.vendor_id = lScript.get<int>("devices." + s + ".vendorid", noerr);
            if (!noerr) break;
        }
        val.index = lScript.get<int>("devices." + s + ".index", noerr);
        if (!noerr) val.index = 0;

        dList.push_back(val);
    }
    //Populate the list of found joysticks
    for (unsigned int i = 0; i < dList.size(); i++) {
        Joystick *cJoy = new Joystick(dList[i]);
        if (!cJoy->isFound()) {
            std::cout << "WARNING: Joystick " << dList[i].name << "[" << dList[i].index << "] is not found.\n";
            delete cJoy;
            return false;
        }

        GLOBAL::joyList.push_back(cJoy);
    }//for

    return true;
}

//Populate a alist of virtual devices defined in user lua file
bool populate_virtual_devices(LuaScript &lScript) {
    std::vector<std::array<int, 2>> dList;
    std::array<int, 2> val;
    for (std::string s : lScript.getTableKeys("v_devices")) {
        bool noerr;
        val[0] = lScript.get<int>("v_devices." + s + ".buttons", noerr);
        if (!noerr) break;
        val[1] = lScript.get<int>("v_devices." + s + ".axes", noerr);
        if (!noerr) break;

        dList.push_back(val);
    }

    //Create and populate the list of user defined virtual devices
    for (unsigned int i = 0; i < dList.size(); i++) {
        CVirtualJoy *vJoy = new CVirtualJoy(dList[i][0], dList[i][1]);
        if (!vJoy->isOpen()) return false;
        GLOBAL::vJoyList.push_back(vJoy);
    }//for

    return true;
}

//Initialize lua functions
void link_lua_functions(LuaScript &lScript) {
    lScript.pushcfunction(l_send_vjoy_button_event, "send_button_event");
    lScript.pushcfunction(l_send_vjoy_axis_event, "send_axis_event");
    lScript.pushcfunction(l_send_keyboard_event, "send_keyboard_event");
    lScript.pushcfunction(l_get_joy_button_status, "get_button_status");
    lScript.pushcfunction(l_get_joy_axis_status, "get_axis_status");
    lScript.pushcfunction(l_get_vjoy_button_status, "get_vjoy_button_status");
    lScript.pushcfunction(l_get_vjoy_axis_status, "get_vjoy_axis_status");
}


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Please type the path of your lua file.\n";
        std::cout << "I.e. 'wejoy config.lua'\n";
        return 0;
    }

    //Open the user lua file.
    LuaScript lScript(argv[1]);
    if (!lScript.isOpen()) return 0;

    if (!populate_devices(lScript)) exit(0);

    GLOBAL::vKeyboard.init();

    if (!populate_virtual_devices(lScript)) exit(0);
    link_lua_functions(lScript);

    std::cout << "Press 'q' and then 'ENTER' to quit!\n";
    std::thread threadUpdate(updateThread, std::ref(lScript));

    while (getchar() != 'q');
    bPoll = false,
            threadUpdate.join();
    sleep(1);

    return 0;
}

