cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)
project(msdf-c VERSION 0.1.0 LANGUAGES CXX C)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

include(FetchContent)
FetchContent_Declare(freetype
		URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
		URL_MD5 c50a3c9e5e62bdc938a6e1598a782947
		)
FetchContent_GetProperties(freetype)
if(NOT freetype_POPULATED)
	set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
	set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
	set(CMAKE_DISABLE_FIND_PACKAGE_PNG TRUE)
	set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)
	FetchContent_Populate(freetype)
	add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR})
	set_target_properties(freetype PROPERTIES FOLDER External)
endif()

add_executable(msdf main.cpp msdf.c msdf.h stb_image_write.h)
target_include_directories(msdf PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_features(msdf PUBLIC cxx_std_17 c_std_11)
target_link_libraries(msdf PUBLIC freetype)
