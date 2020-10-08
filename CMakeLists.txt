project(mowm)

include_directories(/usr/local/include /usr/include)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

include(FindPkgConfig)

FILE(GLOB cppsources *.cpp)

add_executable(mowm src/main.cpp src/config.cpp src/monitor_manager.cpp
	src/globals.cpp src/event_handler.cpp)
target_link_libraries(mowm libX11.so libXinerama.so)

install(TARGETS ${PROJECT_NAME})
