cmake_minimum_required(VERSION 3.20)

project(OledDriver)
add_library(driverSsd1315 INTERFACE)

enable_language(C)

target_compile_definitions(driverSsd1315 INTERFACE
)

target_include_directories(driverSsd1315 INTERFACE
    ./
)

target_sources(driverSsd1315 INTERFACE
    ./ssd1315_i2c.c
)
