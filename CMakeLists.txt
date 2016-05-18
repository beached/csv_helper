cmake_minimum_required( VERSION 2.8.0 )
project( csv_helper_prj )

include( ExternalProject )

set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost 1.59.0 REQUIRED COMPONENTS system filesystem unit_test_framework date_time )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion" )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations" )
endif( )

ExternalProject_Add( 
	header_libraries_prj
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/3rdparty/header_libraries"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
 )

externalproject_add( 
	memory_mapped_file_prj
	GIT_REPOSITORY "https://github.com/beached/memory_mapped_file.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/memory_mapped_file"
	GIT_TAG "master"
	PATCH_COMMAND ""
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	TEST_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

set( HEADER_FOLDER "include" )
include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
		${HEADER_FOLDER}/data_algorithms.h
		${HEADER_FOLDER}/data_cell.h
		${HEADER_FOLDER}/data_column.h
		${HEADER_FOLDER}/data_common.h
		${HEADER_FOLDER}/data_container.h
		${HEADER_FOLDER}/data_table.h
		${HEADER_FOLDER}/data_types.h
		${HEADER_FOLDER}/defs.h
		${HEADER_FOLDER}/string_helpers.h
		${HEADER_FOLDER}/variant.h
)

set( SOURCE_FILES
		data_cell.cpp
		data_column.cpp
		data_table.cpp
		string_helpers.cpp
		variant.cpp
)

add_library( csv_helper STATIC ${HEADER_FILES} ${SOURCE_FILES} )

add_dependencies( csv_helper header_libraries_prj memory_mapped_file_prj )

install( TARGETS csv_helper DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/csv_helper )

