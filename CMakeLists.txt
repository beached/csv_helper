cmake_minimum_required( VERSION 2.8.0 )
project( csv_helper )

include( ExternalProject )

set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost 1.59.0 REQUIRED COMPONENTS system filesystem unit_test_framework date_time )
set( Boost_Libs ${BOOST_Libraries} -lboost_system -lboost_regex -lboost_filesystem -lboost_program_options -lboost_iostreams -lboost_date_time )

message( WARNING "Current Compiler:" ${CMAKE_CXX_COMPILER_ID} )
if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	message( WARNING "Using Visual Studio" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 /MP" )
	set( Boost_Libs )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	message( WARNING "Using Clang" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion" )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	message( WARNING "Using G++" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations" )
endif( )

ExternalProject_Add( 
	header_libraries
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/header_libraries"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
 )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

set( HEADER_FILES
		cstring.h
		data_algorithms.h
		data_cell.h
		data_column.h
		data_common.h
		data_container.h
		data_table.h
		data_types.h
		defs.h
		memorymappedfile.h
		new_helper.h
		string_helpers.h
		variant.h
 )

set( SOURCE_FILES
		data_cell.cpp
		data_column.cpp
		data_table.cpp
		memorymappedfile.cpp
		string_helpers.cpp
		variant.cpp
 )

add_library( csv_helper SHARED ${HEADER_FILES} ${SOURCE_FILES} )
add_library( csv_helper_static STATIC ${HEADER_FILES} ${SOURCE_FILES} )
add_dependencies( csv_helper header_libraries )
install( TARGETS csv_helper DESTINATION lib )
install( TARGETS csv_helper_static DESTINATION lib )

foreach( CUR_PREFIX ${HEADER_FILES} )
	install( FILES "${CUR_PREFIX}" DESTINATION include/daw/csv_helper )
endforeach( CUR_PREFIX )


