add_library(common STATIC
        options.c
        options.h
        protocol.c
        protocol.h
        sig_handler.c
        sig_handler.h
        socket.c
        socket.h
        util.c
        util.h
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})