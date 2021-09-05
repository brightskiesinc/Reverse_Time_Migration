set(SOURCES_BASE

        # MEMORY TRACKER
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/mem_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/data-units/string_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/managers/memory_tracker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/mem_utils.cpp

        # MEMORY TRACKER
        ${CMAKE_CURRENT_SOURCE_DIR}/managers/memory_allocator.cpp

        ${SOURCES_BASE}
        PARENT_SCOPE
        )
