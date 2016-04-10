cmake_minimum_required(VERSION 2.8.0)
project(csv_helper)

include(ExternalProject)

set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost 1.59.0 REQUIRED COMPONENTS system filesystem unit_test_framework date_time )
set( Boost_Libs ${BOOST_Libraries} -lboost_system -lboost_regex -lboost_filesystem -lboost_program_options -lboost_iostreams -lboost_date_time )

MESSAGE( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	MESSAGE( WARNING "Using Visual Studio" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 /MP" )
	set( Boost_Libs )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	MESSAGE( WARNING "Using Clang" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion" )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	MESSAGE( WARNING "Using G++" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations" )
ENDIF()

ExternalProject_Add( 
	header_libraries
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/header_libraries"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_COMMAND ""
	TEST_COMMAND ""
)

include_directories( "${CMAKE_BINARY_DIR}/3rdparty/" )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

set(SOURCE_FILES
		cstring.h
		data_algorithms.h
		data_cell.cpp
		data_cell.h
		data_column.h
		data_column.cpp
		data_common.h
		data_container.h
		data_table.cpp
		data_table.h
		data_types.h
		defs.h
		memorymappedfile.cpp
		memorymappedfile.h
		new_helper.h
		string_helpers.cpp
		string_helpers.h
		variant.cpp
		variant.h
)

#3add_library (csv_helper STATIC ${SOURCE_FILES} )
add_library (csv_helper SHARED ${SOURCE_FILES} )
add_library (csv_helper_static STATIC ${SOURCE_FILES} )
add_dependencies( csv_helper header_libraries )


