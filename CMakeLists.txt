project(mowm)

include_directories(/usr/local/include /usr/include)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

include(FindPkgConfig)

FILE(GLOB cppsources *.cpp)
file(GLOB mowm_SRC 
  "src/*.hpp"
  "src/layouts/*.hpp"
  "src/*.cpp"
  "src/layouts/*.cpp")

add_executable(mowm ${mowm_SRC})
target_link_libraries(mowm libX11.so libXinerama.so)

install(TARGETS ${PROJECT_NAME})
