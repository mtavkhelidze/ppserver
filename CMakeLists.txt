cmake_minimum_required(VERSION 3.0)
project(ping-pong LANGUAGES C VERSION 1.0)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

set(CMAKE_C_STANDARD 11)

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

if (${CMAKE_COMPILER_IS_GNUCC})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Werror")
endif ()

add_executable(${PROJECT_NAME}
        main.c
        socket.c
        socket.h
        server.c
        server.h
        response.c
        response.h
        tpool.c
        tpool.h
        signal.c
        signal.h
        options.c options.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
