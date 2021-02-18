add_executable(texas TexasServer.cpp)

target_link_libraries(texas tcore)

set_target_properties(texas
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)