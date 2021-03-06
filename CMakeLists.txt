cmake_minimum_required(VERSION 3.13)

project(ping-pong LANGUAGES C VERSION 1.1)
set(PROJECT_AUTHOR "Misha Tavkhelidze <misha.tavkhelidze@gmail.com>")

set(CMAKE_C_STANDARD 11)

if (${CMAKE_COMPILER_IS_GNUCC})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Werror")
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if (NOT DEFINED PP_PORT)
    # Right after com-bardac-dw in /etc/services
    set(PP_PORT "48557")
endif ()

if (NOT DEFINED PP_HOST)
    set(PP_HOST "0.0.0.0")
endif ()

if (NOT DEFINED PP_BACKLOG)
    set(PP_BACKLOG 10)
endif ()

if (NOT DEFINED PP_TTL)
    set(PP_TTL 60)
endif ()

if (NOT DEFINED PP_CLIENT_REQ)
    set(PP_CLIENT_REQ "PING")
endif ()

if (NOT DEFINED PP_SERVER_RES)
    set(PP_SERVER_RES "PONG")
endif ()

if (NOT DEFINED PP_SERVER_HUP)
    set(PP_SERVER_HUP "BYE")
endif ()

configure_file(config.h.cmake config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
