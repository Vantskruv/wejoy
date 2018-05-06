#ifndef CVIRTUAL_JOY_H
#define CVIRTUAL_JOY_H

#include <vector>
#include "suinput.h"

class CVirtualJoy {
private:
    int deviceid;
    int fd;                                    //Device handler id
    uint64_t buttonFlags = 0;                //Curent values of all buttons
    std::vector<int> axesData;                //Current values of all axis

public:
    void set_button_flags(uint64_t _b) { buttonFlags = _b; };        //Set buttons to on or off
    const uint64_t &get_button_flags() { return buttonFlags; };        //Get the status of button states.
    int get_button_status(int);                                                //Get defined button state
    void set_axis_data(int t, int v) { axesData[t] = v; };        //Set defined axis data
    int get_axis_status(int t) { return axesData[t]; };        //Get defined axis data


    void send_button_event(int, int);

    void send_axis_event(int, int);

    int getDeviceid();

    bool isOpen();

    CVirtualJoy(unsigned int, unsigned int);    //buttons, axes
    ~CVirtualJoy();
};


#endif
