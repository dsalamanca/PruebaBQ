cmake_minimum_required(VERSION 2.8)
project( PruebaBQ )
find_package( OpenCV REQUIRED )

add_executable( PruebaBQ PruebaBQ.cpp )
target_link_libraries( PruebaBQ ${OpenCV_LIBS} )
set(CMAKE_CXX_FLAGS "-O2 -Wall")
