
if(USE_OpenCV)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(csv_to_png csv_to_png.cpp)
target_link_libraries(csv_to_png ${OpenCV_LIBS})
endif()

add_executable(compare_csv compare_csv.cpp)

add_library(
	cmd-parser
	SHARED
	cmd_parser_utils.cpp
	)

add_library(
		general-utils
		SHARED
		write_utils.cpp
		read_utils.cpp
)

add_library(
		noise-filtering
		SHARED
		noise_filtering.cpp
)

add_executable(post-process post_process.cpp)
target_link_libraries(post-process seis-io general-utils segy-tools noise-filtering)