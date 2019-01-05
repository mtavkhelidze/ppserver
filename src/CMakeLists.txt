add_executable(${PROJECT_NAME}
        options.c
        options.h
        main.c
        protocol.c
        protocol.h
        response.c
        response.h
        server.c
        server.h
        sig_handler.c
        sig_handler.h
        socket.c
        socket.h
        tpool.c
        tpool.h
        util.c
        util.h
        )

target_link_libraries(${PROJECT_NAME} Threads::Threads)
