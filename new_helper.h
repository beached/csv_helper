#pragma once

#include <exception>
#include "exception.h"
#include "header_libraries/daw_string.h"
#include <memory>

template<typename T, typename... Args>
T* new_throw( Args&&... args) {
	std::unique_ptr<T> result;
	try {
		result = std::make_unique<T>( std::forward<Args>( args )... );
	} catch( ... ) {
		std::rethrow_exception( std::current_exception( ) );
	}
	daw::exception::daw_throw_on_false( result, "Error allocating" );
	return result.release( );
}

template<typename T>
T* new_array_throw( const size_t size ) {
	T* result = nullptr;
	try {
		result = new T[size];
	} catch( ... ) {
		std::rethrow_exception( std::current_exception( ) );
	}
	daw::exception::daw_throw_on_null( result, daw::string::string_join( "Error allocating ", size, " items" ) );
	return result;
}
