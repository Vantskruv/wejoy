//
// Created by sanjay on 5/8/18.
//

#ifndef WEJOY_CONTROLLER_H
#define WEJOY_CONTROLLER_H


#include "suinput.h"
#include <vector>
#include <string>

class Controller {

private:

    int deviceid;
    int fd;                                    //Device handler id
    uint64_t buttonFlags = 0;                //Curent values of all buttons
    std::vector<int> axesData;                //Current values of all axis
    char name[16];
    std::string eventname;
    std::string jsname;

public:
    void set_button_flags(uint64_t _b) { buttonFlags = _b; };        //Set buttons to on or off
    const uint64_t &get_button_flags() { return buttonFlags; };        //Get the status of button states.
    void set_axis_data(int t, int v) { axesData[t] = v; };        //Set defined axis data


    void send_button_event(int, int);

    void send_axis_event(int, int);

    int getDeviceid();

    bool isOpen();

    Controller(unsigned int, unsigned int, __u16, __u16, std::string);    //buttons, axesMapping
    ~Controller();
};


#endif //WEJOY_CONTROLLER_H
