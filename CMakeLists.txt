cmake_minimum_required( VERSION 2.8.12 )
project( csv_helper_prj )

include( ExternalProject )

set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost 1.58.0 REQUIRED COMPONENTS date_time )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	find_package( Threads REQUIRED )
	if( THREADS_HAVE_PTHREAD_ARG )
		set( COMPILER_SPECIFIC_LIBS, "-lpthread" )
	endif()
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -Weverything -Wno-c++98-compat -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)		
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -Wall -Wno-deprecated-declarations)
	endif()
endif( )

externalproject_add(
    header_libraries_prj
    GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( ${HEADER_FOLDER} )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

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
	${SOURCE_FOLDER}/data_cell.cpp
	${SOURCE_FOLDER}/data_column.cpp
	${SOURCE_FOLDER}/data_table.cpp
	${SOURCE_FOLDER}/string_helpers.cpp
	${SOURCE_FOLDER}/variant.cpp
)

add_library( csv_helper STATIC ${HEADER_FILES} ${SOURCE_FILES} )
add_dependencies( csv_helper header_libraries_prj )

install( TARGETS csv_helper DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/csv_helper )

