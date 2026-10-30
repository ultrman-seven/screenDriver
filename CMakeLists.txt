cmake_minimum_required(VERSION 3.20)

project(OledDriver)
enable_language(C)

add_library(driverSsd1315 INTERFACE)

target_compile_definitions(driverSsd1315 INTERFACE
)

target_include_directories(driverSsd1315 INTERFACE
./
)

target_sources(driverSsd1315 INTERFACE
./ssd1315_i2c.c
)

add_library(driverSsd1312 INTERFACE)

target_include_directories(driverSsd1312 INTERFACE
    ./
)
target_sources(driverSsd1312 INTERFACE
    ./ssd1312_i2c.c
)
