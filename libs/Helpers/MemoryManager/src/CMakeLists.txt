set(MEMORY-MANAGER-SOURCES

        # MEMORY TRACKER
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/mem_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/string_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/managers/memory_tracker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/mem_utils.cpp

        # MEMORY TRACKER
        ${CMAKE_CURRENT_SOURCE_DIR}/managers/memory_allocator.cpp
        )

add_library(MEMORY-MANAGER STATIC ${MEMORY-MANAGER-SOURCES})