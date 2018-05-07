cmake_minimum_required(VERSION 3.6)
project(wejoy)
set(CMAKE_CXX_STANDARD 11)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/input_events.h
        COMMAND cat /usr/include/linux/input.h /usr/include/linux/input-event-codes.h | python ${CMAKE_CURRENT_SOURCE_DIR}/make-event-names.py > $@
)

set(SOURCE_FILES input_events.h main.cc suinput.c joystick.cc LuaScript.cc CVirtualJoy.cc CVirtualKeyboard.cc)
add_executable(wejoy ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(wejoy udev pthread X11 lua5.2 evdev)