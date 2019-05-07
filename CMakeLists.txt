cmake_minimum_required (VERSION 2.6)
project (Armagetron)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -I/usr/X11R6/include -I/usr/pkg/include")
set(LDFLAGS " -L/usr/X11R6/lib -L/usr/pkg/lib")
set(LDLIBS "-lSOIL -lglut -lGLU -lGL -lm")


add_library(Image image.c)
add_library(Soil SOIL)
add_executable(Armagetron main)

target_link_libraries(Armagetron ${LDLIBS})
target_link_libraries(Armagetron Image)