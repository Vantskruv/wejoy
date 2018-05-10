#include <unistd.h> //sleep
#include <thread>    //thread

#include "LuaScript.h"
#include "global.h"
#include <libevdev-1.0/libevdev/libevdev.h>
#include <algorithm>

bool bPoll = true;

void updateThread(LuaScript &lScript) {
    //Sleep one second to give the X11 system time to adapt.
    sleep(1);

    int rc;
    while (bPoll) {
        for (Joystick *joy : GLOBAL::joyList) {
            struct input_event ev;
            rc = libevdev_next_event(joy->get_dev(), LIBEVDEV_READ_FLAG_NORMAL, &ev);
            if (rc == 0) {
                joy->handleEvent(ev);
                if (ev.type == EV_KEY) {
                    int index = joy->get_button_index(ev.code);
                    lScript.call_value_function("button_event", joy->getLuaName(), index,
                                                ev.value);
                    lScript.call_value_function("button_event_code", joy->getLuaName(), ev.code, ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_b" + std::to_string(ev.code) + "_event",
                                                 ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_b" + std::to_string(index) + "_event",
                                                 ev.value);
                    const char *name = libevdev_event_code_get_name(EV_KEY, ev.code);
                    if (name != NULL) {
                        lScript.call_device_function(joy->getLuaName() + name + "_event",
                                                     ev.value);
                    }

                } else if (ev.type == EV_ABS) {
                    int index = joy->get_axis_index(ev.code);
                    lScript.call_value_function("axis_event", joy->getLuaName(), index,
                                                ev.value);
                    lScript.call_value_function("axis_event_code", joy->getLuaName(), ev.code, ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_a" + std::to_string(ev.code) + "_event",
                                                 ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_a" + std::to_string(index) + "_event",
                                                 ev.value);
                    const char *name = libevdev_event_code_get_name(EV_ABS, ev.code);
                    if (name != NULL) {
                        lScript.call_device_function(joy->getLuaName() + name + "_event",
                                                     ev.value);
                    }
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
int l_get_joy_button_status_code(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_button_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_button_status(joystick->get_button_index(type));
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_status_code(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_axis_status(joystick->get_axis_index(type));
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_button_status(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int index = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_button_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    int status = GLOBAL::joyList[id]->get_button_status(index);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_status(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int index = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    int status = GLOBAL::joyList[id]->get_axis_status(index);
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
int l_get_joy_axis_min(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int index = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_axis_min(joystick->get_axis_type(index));
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_min_code(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_axis_min(type);
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_axis_max(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int index = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_axis_max(joystick->get_axis_type(index));
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_axis_max_code(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    int type = lua_tonumber(L, 2);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_axis_max(type);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_count(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_num_axes();
    lua_pushnumber(L, status);
    return 1;
}

int l_get_joy_button_count(lua_State *L) {
    unsigned int id = lua_tonumber(L, 1);
    if (id >= GLOBAL::joyList.size()) {
        std::cout << "ERROR get_joy_axis_status: Device " << id << " does not exist.\n";
        lua_pushnumber(L, -1);
        return 1;
    }
    Joystick *joystick = GLOBAL::joyList[id];
    int status = joystick->get_num_buttons();
    lua_pushnumber(L, status);
    return 1;
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
    std::vector<std::string> devices = lScript.getTableKeys("devices");
    std::sort(devices.begin(), devices.end());
    for (const std::string &s : devices) {
        val = LuaStick();
        val.lua_name = s;
        bool noerr;
        val.name = lScript.get<std::string>("devices." + s + ".name", noerr);
        if (!noerr) {
            val.name = "";
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
            if (dList[i].name.empty()) {
                std::cout << "WARNING: Joystick vid:" << dList[i].vendor_id << " pid:" << dList[i].product_id
                          << " at index " << dList[i].index << " is not found.\n";
            } else {
                std::cout << "WARNING: Joystick " << dList[i].name << "[" << dList[i].index << "] is not found.\n";
            }
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
    for (const std::string &s : lScript.getTableKeys("v_devices")) {
        bool noerr;
        val[0] = lScript.get<int>("v_devices." + s + ".buttons", noerr);
        if (!noerr) break;
        val[1] = lScript.get<int>("v_devices." + s + ".axes", noerr);
        if (!noerr) break;

        dList.push_back(val);
    }

    //Create and populate the list of user defined virtual devices
    for (auto &i : dList) {
        auto *vJoy = new CVirtualJoy(i[0], i[1]);
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
    lScript.pushcfunction(l_get_joy_axis_min, "get_axis_min");
    lScript.pushcfunction(l_get_joy_axis_min_code, "get_axis_min_code");
    lScript.pushcfunction(l_get_joy_axis_max, "get_axis_max");
    lScript.pushcfunction(l_get_joy_axis_max_code, "get_axis_max_code");
    lScript.pushcfunction(l_get_joy_axis_count, "get_axis_count");
    lScript.pushcfunction(l_get_joy_button_count, "get_button_count");
    lScript.pushcfunction(l_get_joy_button_status_code, "get_button_status_code");
    lScript.pushcfunction(l_get_joy_axis_status_code, "get_axis_status_code");
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
    lScript.call_main_function();
    std::cout << "Press 'q' and then 'ENTER' to quit!\n";
    std::thread threadUpdate(updateThread, std::ref(lScript));

    while (getchar() != 'q');
    bPoll = false,
            threadUpdate.join();
    sleep(1);

    return 0;
}

