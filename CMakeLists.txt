cmake_minimum_required(VERSION 2.8.0)
project(csv_helper)
include(ExternalProject)


find_package(Boost 1.55.0 COMPONENTS system filesystem unit_test_framework REQUIRED)

set( THIRD_PARTY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/third_party" )

set(CLANG_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")
set(GCC_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Wall" )

if(CMAKE_CXX_COMPILER_ID "Clang")
	set( CMAKE_CXX_FLAGS "{$CLANG_CMAKE_CXX_FLAGS}" )
elseif(CMAKE_CXX_COMPILER_ID "GNU")
	set( CMAKE_CXX_FLAGS "{$GCC_CMAKE_CXX_FLAGS}" )
endif( )

ExternalProject_Add( header_libraries
    GIT_REPOSITORY https://github.com/beached/header_libraries#
	SOURCE_DIR "${THIRD_PARTY_FILES}/header_libraries"
    INSTALL_COMMAND ""
)

set(SOURCE_FILES
		cstring.h
		data_algorithms.h
		data_cell.cpp
		data_cell.h
		data_column.h
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


include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${THIRD_PARTY_FILES}/header_libraries" )

add_library (csv_helper STATIC ${SOURCE_FILES} )
add_dependencies( csv_helper header_libraries )

