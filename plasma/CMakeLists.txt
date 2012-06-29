
cmake_minimum_required(VERSION 2.8)

find_package(Qt4 REQUIRED)

include_directories(common)

add_subdirectory(metaclient)
add_subdirectory(updater)
add_subdirectory(watchdog)
add_subdirectory(meta)
