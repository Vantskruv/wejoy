cmake_minimum_required(VERSION 3.0)

project(cmake_test)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Make test executable
set(TEST_SOURCES test_main.cpp controller.cpp suinput.c)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests Catch udev pthread X11 lua5.2 evdev)