cmake_minimum_required(VERSION 3.16)
project(Asciixel LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third_party/freetype-2.14.3 EXCLUDE_FROM_ALL)

set(FFMPEG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/build/ffmpeg-install" CACHE PATH
    "Installation directory of the FFmpeg submodule build")
if(NOT EXISTS "${FFMPEG_ROOT}/include/libavformat/avformat.h" OR
   NOT EXISTS "${FFMPEG_ROOT}/include/libavutil/samplefmt.h")
    message(FATAL_ERROR "FFmpeg installation is incomplete. Run scripts/build-ffmpeg.ps1 first, or set FFMPEG_ROOT.")
endif()

foreach(component avformat avcodec avutil swscale)
    find_library(FFMPEG_${component}_LIBRARY NAMES ${component}
        PATHS "${FFMPEG_ROOT}/lib" NO_DEFAULT_PATH REQUIRED)
endforeach()

add_library(asciixel_core STATIC
	src/core/image_sampler.cpp
	src/core/charset_builder.cpp
	src/config/charset_config.cpp
	src/io/image_loader.cpp
	src/io/frame_converter.cpp
)

target_link_libraries(asciixel_core PRIVATE freetype
    ${FFMPEG_avformat_LIBRARY}
    ${FFMPEG_avcodec_LIBRARY}
    ${FFMPEG_avutil_LIBRARY}
    ${FFMPEG_swscale_LIBRARY})

target_include_directories(asciixel_core PRIVATE "${FFMPEG_ROOT}/include")

target_include_directories(asciixel_core
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(CTest)

if(BUILD_TESTING)
	add_executable(test_image_sampler
		tests/test_image_sampler.cpp
	)
	target_link_libraries(test_image_sampler PRIVATE asciixel_core)

	add_test(NAME image_sampler COMMAND test_image_sampler)

	add_executable(test_image_loader tests/test_image_loader.cpp)
	target_link_libraries(test_image_loader PRIVATE asciixel_core)
	add_test(NAME image_loader COMMAND test_image_loader
		"${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/red_green.png"
		"${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

	add_executable(test_frame_converter tests/test_frame_converter.cpp)
	target_link_libraries(test_frame_converter PRIVATE asciixel_core)
	target_include_directories(test_frame_converter PRIVATE "${FFMPEG_ROOT}/include")
	add_test(NAME frame_converter COMMAND test_frame_converter)

	file(GLOB FFMPEG_DLLS "${FFMPEG_ROOT}/bin/*.dll")
	if(FFMPEG_DLLS)
		add_custom_command(TARGET test_image_loader POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FFMPEG_DLLS}
				$<TARGET_FILE_DIR:test_image_loader>
			COMMAND_EXPAND_LISTS)
		add_custom_command(TARGET test_frame_converter POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FFMPEG_DLLS}
				$<TARGET_FILE_DIR:test_frame_converter>
			COMMAND_EXPAND_LISTS)
	endif()

	add_executable(test_charset_config tests/test_charset_config.cpp)
	target_link_libraries(test_charset_config PRIVATE asciixel_core)
	add_test(NAME charset_config COMMAND test_charset_config)

	add_executable(test_charset_builder tests/test_charset_builder.cpp)
	target_link_libraries(test_charset_builder PRIVATE asciixel_core)
	if(WIN32 AND EXISTS "C:/Windows/Fonts/consola.ttf")
		add_test(NAME charset_builder COMMAND test_charset_builder "C:/Windows/Fonts/consola.ttf")
	endif()
endif()

