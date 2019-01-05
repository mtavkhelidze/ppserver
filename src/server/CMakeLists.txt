add_executable(${PROJECT_NAME}
        ppserver.c
        response.c
        response.h
        server.c
        server.h
        tpool.c
        tpool.h
        )


target_compile_definitions(${PROJECT_NAME} PUBLIC PP_SERVER=1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} "../common")

target_link_libraries(${PROJECT_NAME} common)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
