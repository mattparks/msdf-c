cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)
project(msdf-c VERSION 0.1.0 LANGUAGES C)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

option(BUILD_MSDFC_EXAMPLE "Build MSDF-C example project" ON)

add_library(msdf msdf.h stb_truetype.h msdf.c)
target_include_directories(msdf PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_features(msdf PUBLIC c_std_99)
install(TARGETS msdf
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/msdf.h
		${CMAKE_CURRENT_SOURCE_DIR}/stb_truetype.h
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
		)

if(BUILD_MSDFC_EXAMPLE)
	add_executable(msdf_gen main.c stb_image_write.h)
	target_link_libraries(msdf_gen msdf)
    install(TARGETS msdf_gen
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            )
endif()
