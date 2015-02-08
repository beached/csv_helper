#pragma once

#include <stdexcept>
#include <string>
#include "daw_string.h"

namespace daw {
	namespace exception {
#define MAKE_DAW_EXCEPTION( EXCEPTION_TYPE ) \
			struct EXCEPTION_TYPE: public std::runtime_error { \
				template<typename StringType> EXCEPTION_TYPE( StringType const & msg ): std::runtime_error( msg ) { } \
					}

		MAKE_DAW_EXCEPTION( NotImplemented );
		MAKE_DAW_EXCEPTION( FatalError );
		MAKE_DAW_EXCEPTION( NullPtrAccessException );
		MAKE_DAW_EXCEPTION( AssertException );
		MAKE_DAW_EXCEPTION( FileException );

#undef MAKE_DAW_EXCEPTION

		template<typename ExceptionType = std::runtime_error, typename StringType>
		void daw_throw( StringType const & msg ) {
			throw ExceptionType( msg );
		}

		template<typename ExceptionType = std::runtime_error, typename StringType>
		void debug_throw( StringType const & msg ) {
#ifndef NODEBUGTHROW
			daw_throw<ExceptionType>( msg );
#else
			return;
#endif
		}

		template<typename ExceptionType = NullPtrAccessException, typename ValueType, typename StringType>
		void dbg_throw_on_null( ValueType* value, StringType const & msg ) {
			if( nullptr == value ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = NullPtrAccessException, typename ValueType, typename StringType>
		ValueType* dbg_throw_on_null_or_return( ValueType* value, StringType const & msg ) {
			if( nullptr == value ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType>
		ValueType& dbg_throw_on_false_or_return( ValueType& value, bool test, StringType const & msg ) {
			if( !test ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		bool dbg_throw_on_false_or_return( bool test, StringType const & msg ) {
			if( !test ) {
				debug_throw<ExceptionType>( msg );
			}
			return true;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		void dbg_throw_on_false( bool test, StringType const & msg ) {
			if( !test ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void dbg_throw_on_false( bool test, StringType const & format, Arg arg, Args... args ) {
			if( !test ) {
				debug_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType>
		ValueType& dbg_throw_on_true_or_return( ValueType& value, bool test, StringType const & msg ) {
			if( test ) {
				debug_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename BoolType, typename StringType>
		bool dbg_throw_on_true_or_return( const BoolType& test, StringType const & msg ) {
			if( test ) {
				debug_throw<ExceptionType>( msg );
			}
			return test;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		void dbg_throw_on_true( bool test, StringType const & msg ) {
			if( test ) {
				debug_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void dbg_throw_on_true( bool test, StringType const & format, Arg arg, Args... args ) {
			if( test ) {
				debug_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
		}


		template<typename ExceptionType = NullPtrAccessException, typename ValueType, typename StringType>
		void daw_throw_on_null( ValueType* value, StringType const & msg ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType, typename Arg, typename... Args>
		void daw_throw_on_null( ValueType* value, StringType format, Arg arg, Args... args ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType>
		ValueType* daw_throw_on_null_or_return( ValueType* value, StringType const & msg ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( msg );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType, typename Arg, typename... Args>
		ValueType* daw_throw_on_null_or_return( ValueType* value, StringType format, Arg arg, Args... args ) {
			if( nullptr == value ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
			return value;
		}

		template<typename ExceptionType = AssertException, typename StringType, typename BoolType, typename Arg, typename... Args>
		bool daw_throw_on_false_or_return( BoolType&& test, StringType format, Arg arg, Args... args ) {
			if( !test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
			return true;
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType>
		ValueType& daw_throw_on_false_or_return( ValueType&& value, BoolType&& test, StringType msg ) {
			if( !test ) {
				daw_throw<ExceptionType>( msg );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType, typename Arg, typename... Args>
		ValueType& daw_throw_on_false_or_return( ValueType&& value, BoolType&& test, StringType format, Arg arg, Args... args ) {
			if( !test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename BoolType, typename StringType>
		void daw_throw_on_false( const BoolType& test, StringType const & msg ) {
			if( !test ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void daw_throw_on_false( bool test, StringType const & format, Arg arg, Args... args ) {
			if( !test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType, typename Arg, typename... Args>
		ValueType& daw_throw_on_true_or_return( ValueType&& value, BoolType&& test, StringType format, Arg arg, Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename BoolType>
		ValueType& daw_throw_on_true_or_return( ValueType&& value, BoolType&& test, StringType msg ) {
			if( test ) {
				daw_throw<ExceptionType>( msg );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename ValueType, typename StringType, typename Arg, typename... Args>
		ValueType& daw_throw_on_true_or_return( ValueType&& value, bool test, StringType const & format, Arg arg, Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
			return std::forward( value );
		}

		template<typename ExceptionType = AssertException, typename StringType>
		bool daw_throw_on_true_or_return( bool test, StringType const & msg ) {
			if( test ) {
				daw_throw<ExceptionType>( msg );
			}
			return false;
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		bool daw_throw_on_true_or_return( bool test, StringType const & format, Arg arg, Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
			return false;
		}

		template<typename ExceptionType = AssertException, typename StringType>
		void daw_throw_on_true( bool test, StringType const & msg ) {
			if( test ) {
				daw_throw<ExceptionType>( msg );
			}
		}

		template<typename ExceptionType = AssertException, typename StringType, typename Arg, typename... Args>
		void daw_throw_on_true( bool test, StringType const & format, Arg arg, Args... args ) {
			if( test ) {
				daw_throw<ExceptionType>( daw::string::string_format( format, arg, args... ) );
			}
		}
	}	// namespace exception
}	// namespace daw
