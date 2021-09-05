# Executable

add_executable(Example_SegyToImage ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(Example_SegyToImage BS-IO)
