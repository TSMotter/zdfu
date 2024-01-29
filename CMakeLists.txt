cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zdfu)

if(NOT (DEFINED SYSBUILD))
  message(FATAL_ERROR "This project must be built with sysbuild.")
endif()

set(SRC_DIR "src")
set(SRC_FILES
    "${SRC_DIR}/main.cpp"
)

target_sources(app PRIVATE ${SRC_FILES})