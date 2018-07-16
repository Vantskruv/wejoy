#include <unistd.h> //sleep
#include <thread>    //thread

#include "LuaScript.h"
#include "global.h"
#include "buttons_ref.h"
#include <libevdev-1.0/libevdev/libevdev.h>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <libudev.h>

bool bPoll = true;

int scale(int x, int in_min, int in_max, int out_min, int out_max) {
    return static_cast<int>(std::floor((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min));
}
void monitorThread() {
    struct udev* udev = udev_new();
    if (!udev) {
        fprintf(stderr, "udev_new() failed\n");
        return;
    }
    struct udev_monitor* mon = udev_monitor_new_from_netlink(udev, "udev");

    udev_monitor_filter_add_match_subsystem_devtype(mon, "input", NULL);
    udev_monitor_enable_receiving(mon);

    int fd = udev_monitor_get_fd(mon);
    while (bPoll) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        if (select(fd+1, &fds, NULL, NULL, NULL) <= 0)
            break;

        if (FD_ISSET(fd, &fds)) {
            struct udev_device* dev = udev_monitor_receive_device(mon);
            const char * devNode = udev_device_get_devnode(dev);
            if (devNode == nullptr) continue;
            const char* action = udev_device_get_action(dev);
            if (strcmp(action,"remove") == 0) {
                //Populate the list of found joysticks
                for (auto &i : GLOBAL::joyList) {
                    if (strcmp(i.second->getPath().c_str(),devNode)) {
                        i.second->closeJoy(GLOBAL::wiimoteList);
                        break;
                    }
                }
            }
            if (strcmp(action,"add") == 0) {
                //Populate the list of found joysticks
                for (auto &i : GLOBAL::joyList) {
                    if (!i.second->isFound()) {
                        i.second->init(GLOBAL::wiimoteList);
                    }
                }
            }
        }
    }
}
void updateThread(LuaScript &lScript) {
    //Sleep one second to give the X11 system time to adapt.
    sleep(1);

    int rc;
    while (bPoll) {
        for (auto pair : GLOBAL::joyList) {
            auto *joy = pair.second;
            if (!joy->isFound()) {
                continue;
            }
            struct input_event ev{};
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
                    if (name != nullptr) {
                        lScript.call_device_function(joy->getLuaName() + name + "_event",
                                                     ev.value);
                    }

                } else if (ev.type == EV_ABS) {
                    int index = joy->get_axis_index(ev.code);
                    int scaled = scale(ev.value, joy->get_axis_min(ev.code), joy->get_axis_max(ev.code), MIN_ABS_VAL,
                                       MAX_ABS_VAL);
                    lScript.call_value_function("axis_event", joy->getLuaName(), index,
                                                ev.value);
                    lScript.call_value_function("axis_event_scaled", joy->getLuaName(), index,
                                                scaled);
                    lScript.call_value_function("axis_event_code", joy->getLuaName(), ev.code, ev.value);
                    lScript.call_value_function("axis_event_code_scaled", joy->getLuaName(), ev.code, scaled);
                    lScript.call_device_function(joy->getLuaName() + "_a" + std::to_string(ev.code) + "_event",
                                                 ev.value);
                    lScript.call_device_function(joy->getLuaName() + "_a" + std::to_string(index) + "_event",
                                                 ev.value);
                    const char *name = libevdev_event_code_get_name(EV_ABS, ev.code);
                    if (name != nullptr) {
                        lScript.call_device_function(joy->getLuaName() + name + "_event",
                                                     ev.value);
                    }
                }
            }
        }
    }

}

//Called from user via lua script
int l_send_keyboard_event(lua_State *L) {
    int type = lua_tounsigned(L, 1);
    int value = lua_tounsigned(L, 2);
    GLOBAL::vKeyboard.send_key_event(type, value);
    return 0;
}

Joystick *getJoystick(lua_State *L, const std::string &f) {
    std::string id = lua_tostring(L, 1);
    auto x = GLOBAL::joyList.find(id);
    if (x == GLOBAL::joyList.end()) {
        std::cout << "ERROR " + f + ": Device " << id << " does not exist.\n";
        return nullptr;
    }
    return x->second;
}

CVirtualJoy *getVJoystick(lua_State *L, const std::string &f) {
    std::string id = lua_tostring(L, 1);
    auto x = GLOBAL::vJoyList.find(id);
    if (x == GLOBAL::vJoyList.end()) {
        std::cout << "ERROR " + f + ": Virtual Device " << id << " does not exist.\n";
        return nullptr;
    }
    return x->second;
}

//Called from user via lua script
int l_get_joy_button_status_code(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_button_status_code");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(L, 2);
    int status = joystick->get_button_status(joystick->get_button_index(type));
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_status_code(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_status");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(L, 2);
    int status = joystick->get_axis_status(joystick->get_axis_index(type));
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_button_status(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_button_status");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int index = lua_tounsigned(L, 2);
    int status = joystick->get_button_status(index);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_status(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_status");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int index = lua_tounsigned(L, 2);
    int status = joystick->get_axis_status(index);
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_axis_min(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_min");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int index = lua_tounsigned(L, 2);
    int status = joystick->get_axis_min(joystick->get_axis_type(index));
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_min_code(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_min_code");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(L, 2);
    int status = joystick->get_axis_min(type);
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_axis_max(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_max");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int index = lua_tounsigned(L, 2);
    int status = joystick->get_axis_max(joystick->get_axis_type(index));
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_get_joy_axis_max_code(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_max_code");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(L, 2);
    int status = joystick->get_axis_max(type);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_joy_axis_count(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_axis_count");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    int status = joystick->get_num_axes();
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_send_vjoy_axis_event(lua_State *_L) {
    CVirtualJoy *joystick = getVJoystick(_L, "send_vjoy_axis_event");
    if (joystick == nullptr) {
        lua_pushnumber(_L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(_L, 2);
    auto value = static_cast<int>(lua_tointeger(_L, 3));

    joystick->send_axis_event(type, value);

    return 0;
}


int l_get_joy_button_count(lua_State *L) {
    Joystick *joystick = getJoystick(L, "get_joy_button_count");
    if (joystick == nullptr) return 0;
    int status = joystick->get_num_buttons();
    lua_pushnumber(L, status);
    return 1;
}

int l_send_joy_led_event(lua_State *L) {
    Joystick *joystick = getJoystick(L, "send_joy_led_event");
    unsigned int led = lua_tounsigned(L, 2);
    bool state = static_cast<bool>(lua_toboolean(L, 3));
    if (joystick == nullptr) {
        return 0;
    }
    joystick->set_led_state(led, state);
    int status = joystick->get_num_buttons();
    lua_pushnumber(L, status);
    return 1;
}

//Called from user via lua script
int l_send_vjoy_button_event(lua_State *_L) {
    CVirtualJoy *joystick = getVJoystick(_L, "send_vjoy_button_event");
    if (joystick == nullptr) {
        return 0;
    }
    unsigned int type = lua_tounsigned(_L, 2);
    auto value = static_cast<int>(lua_tointeger(_L, 3));
    joystick->send_button_event(type, value);
    return 0;
}

//Called from user via lua script
int l_get_vjoy_button_status(lua_State *L) {
    CVirtualJoy *joystick = getVJoystick(L, "get_vjoy_button_status");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(L, 2);
    int status = joystick->get_button_status(type);
    lua_pushnumber(L, status);
    return 1;
}


//Called from user via lua script
int l_get_vjoy_axis_status(lua_State *L) {
    CVirtualJoy *joystick = getVJoystick(L, "get_vjoy_axis_status");
    if (joystick == nullptr) {
        lua_pushnumber(L, -1);
        return 0;
    }
    unsigned int type = lua_tounsigned(L, 2);
    int status = joystick->get_axis_status(type);
    lua_pushnumber(L, status);
    return 1;
}


//Populate a list of physical devices defined in user lua file
bool populate_devices(LuaScript &lScript) {
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
        val.wiimote = lScript.get<int>("devices." + s + ".wiimote", noerr);
        if (!noerr) {
            val.wiimote = -1;
            val.index = lScript.get<int>("devices." + s + ".index", noerr);
            if (!noerr) val.index = 0;
        } else {
            val.index = -1;
        }
        GLOBAL::luaJoyList.push_back(val);
    }
    //Populate the list of found joysticks
    for (auto &i : GLOBAL::luaJoyList) {
        Joystick *cJoy = new Joystick(i, GLOBAL::wiimoteList);
        GLOBAL::joyList[i.lua_name] = cJoy;
    }

    return true;
}

//Populate a alist of virtual devices defined in user lua file
bool populate_virtual_devices(LuaScript &lScript) {
    unsigned int buttons;
    unsigned int axes;
    for (const std::string &s : lScript.getTableKeys("v_devices")) {
        bool noerr;
        buttons = lScript.get<unsigned>("v_devices." + s + ".buttons", noerr);
        if (!noerr) break;
        axes = lScript.get<unsigned>("v_devices." + s + ".axes", noerr);
        if (!noerr) break;
        auto *vJoy = new CVirtualJoy(buttons, axes, s);
        if (!vJoy->isOpen()) return false;
        GLOBAL::vJoyList[s] = vJoy;
    }

    return true;
}

//Initialize lua functions
void link_lua_functions(LuaScript &lScript) {
    lScript.pushcfunction(l_send_vjoy_button_event, "send_button_event");
    lScript.pushcfunction(l_send_vjoy_axis_event, "send_axis_event");
    lScript.pushcfunction(l_send_keyboard_event, "send_keyboard_event");
    lScript.pushcfunction(l_send_joy_led_event, "send_joy_led_event");
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
    std::thread monitorThreadUpdate(monitorThread);

    while (getchar() != 'q');
    bPoll = false,
            threadUpdate.join();
    sleep(1);

    return 0;
}

