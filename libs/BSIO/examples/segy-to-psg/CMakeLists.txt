# Executable

add_executable(Example_SegyToPSG ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(Example_SegyToPSG BS-IO)
