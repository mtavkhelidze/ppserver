add_executable(${PROJECT_NAME}-client
        ppclient.c
        )

target_compile_definitions(${PROJECT_NAME}-client PUBLIC PP_CLIENT=1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} "../common")

target_link_libraries(${PROJECT_NAME}-client common)
target_link_libraries(${PROJECT_NAME}-client Threads::Threads)
