cmake_minimum_required(VERSION 2.8)

project(spidev-lib)

include_directories(src)

add_library(spidev-lib src/spidev_lib.c)
add_executable(
     spi-testc sample/spidev-testc.c
)
target_link_libraries( spi-testc spidev-lib)

add_library(spidev-lib++ src/spidev_lib++.cc)
add_library(spidev-lib++ src/spidev_lib++.cc src/spidev_lib++.h)
target_include_directories(spidev-lib++ INTERFACE src)

add_dependencies(spi-testcpp spidev-lib++)
add_dependencies(spi-testc spidev-lib)

INSTALL(TARGETS  spidev-lib  spidev-lib++
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
INSTALL( FILES src/spidev_lib.h DESTINATION include)
INSTALL( FILES src/spidev_lib++.h DESTINATION include)

