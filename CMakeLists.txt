cmake_minimum_required(VERSION 4.0)

project(wejoy)


find_package(PkgConfig REQUIRED)
find_package(Lua REQUIRED)

pkg_search_module(LIBEVDEV REQUIRED libevdev)
pkg_check_modules(LIBUDEV REQUIRED libudev)


add_executable(wejoy)

target_sources(wejoy
  PRIVATE
    suinput.c
    joystick.cc
    CKeyboard.cc
    LuaScript.cc
    CVirtualJoy.cc
    CVirtualKeyboard.cc
    main.cc
)

target_include_directories(wejoy PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
target_include_directories(wejoy PRIVATE ${LIBUDEV_INCLUDE_DIRS})
target_include_directories(wejoy PRIVATE ${LUA_INCLUDE_DIR})

target_link_libraries(wejoy PRIVATE ${LIBEVDEV_LIBRARIES})
target_link_libraries(wejoy PRIVATE ${LIBUDEV_LIBRARIES})
target_link_libraries(wejoy PRIVATE ${LUA_LIBRARIES})

install(TARGETS wejoy)
