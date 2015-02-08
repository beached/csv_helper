cmake_minimum_required(VERSION 2.8.0)

project(csv_helper)

find_package(Boost 1.55.0 COMPONENTS system filesystem unit_test_framework REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -fopenmp -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-unused-function -Wno-unused-member-function")


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

include_directories( header_libraries/ )

add_library (csv_helper STATIC ${SOURCE_FILES})

#add_executable( csv_helper_test_bin test.cpp )
#target_link_libraries( csv_helper_test_bin csv_helper ${Boost_LIBRARIES} )

enable_testing( )
#add_test( csv_helper_test, csv_helper_test_bin )


