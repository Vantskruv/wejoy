set(CMAKE_CXX_STANDARD 11)

set(TEST_SOURCE_FILES test.cpp controller.cpp suinput.c)
add_executable(wejoy_tests ${TEST_SOURCE_FILES})
TARGET_LINK_LIBRARIES(wejoy_tests udev pthread X11 lua5.2 evdev)
