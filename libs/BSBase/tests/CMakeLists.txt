set(TESTFILES_BASE test_main.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/configurations)

enable_testing()
add_executable(bs-base-tests ${TESTFILES_BASE})
target_link_libraries(bs-base-tests BS-BASE)
