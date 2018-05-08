cmake_minimum_required(VERSION 3.10)
project(wejoy)
add_subdirectory(tests)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/input_events.h
        COMMAND cat /usr/include/linux/input.h /usr/include/linux/input-event-codes.h | python ${CMAKE_CURRENT_SOURCE_DIR}/make-event-names.py > $@
)

set(LIB_SOURCE_FILES
        input_events.h
        suinput.c
        joystick.cc
        LuaScript.cc
        CVirtualJoy.cc
        CVirtualKeyboard.cc)
add_library(libwejoy ${LIB_SOURCE_FILES})

target_compile_features(libwejoy PUBLIC cxx_std_11)
target_include_directories(libwejoy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(EXE_SOURCE_FILES main.cc)
add_executable(wejoy ${EXE_SOURCE_FILES})

target_compile_features(wejoy PUBLIC cxx_std_11)

TARGET_LINK_LIBRARIES(wejoy udev pthread X11 lua5.2 evdev libwejoy)
