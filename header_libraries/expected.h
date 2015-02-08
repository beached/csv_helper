#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include "daw_traits.h"
#include "workarounds.h"

namespace daw {
	template<class ValueType>
	class Expected {
		enum class ExpectedSource { value, exception, none };
		ValueType m_value;
		std::exception_ptr m_exception;
		ExpectedSource m_source;
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		Expected( ) : m_value( ), m_exception( ), m_source( ExpectedSource::none ) { }

		Expected( Expected const & other ) = default;
		Expected& operator=(Expected const & rhs) = default;

		Expected( Expected&& other ) noexcept:
		m_value( std::move( other.m_value ) ),
			m_exception( std::move( other.m_exception ) ),
			m_source( std::move( other.m_source ) ) { }

		Expected& operator=(Expected && rhs) noexcept {
			if( this != &rhs ) {
				m_value = std::move( rhs.m_value );
				m_exception = std::move( rhs.m_exception );
				m_source = std::move( rhs.m_source );
			}
			return *this;
		}

		~Expected( ) = default;

		bool operator==(Expected const & rhs) const noexcept {
			return rhs.m_value == m_value && rhs.m_exception == m_exception && rhs.m_source == m_source;
		}

			//////////////////////////////////////////////////////////////////////////
			/// Summary: With value
			//////////////////////////////////////////////////////////////////////////
			Expected( ValueType value ) noexcept: m_value( std::move( value ) ), m_exception( ), m_source( ExpectedSource::value ) { }

		template<class ExceptionType>
		static Expected<ValueType> from_exception( ExceptionType const & exception ) {
			if( typeid(exception) != typeid(ExceptionType) ) {
				throw std::invalid_argument( "slicing detected" );
			}
			return from_exception( std::make_exception_ptr( exception ) );
		}

		static Expected<ValueType>
			from_exception( std::exception_ptr except_ptr ) {
			Expected<ValueType> result;
			result.m_source = Expected<ValueType>::ExpectedSource::exception;
			new(&result.m_exception) std::exception_ptr( std::move( except_ptr ) );
			return result;
		}

		static Expected<ValueType> from_exception( ) {
			return from_exception( std::current_exception( ) );
		}

		bool has_value( ) const noexcept {
			return ExpectedSource::value == m_source;
		}

			ValueType& get( ) {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			return m_value;
		}

		ValueType const & get( ) const {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
			return m_value;
		}

		std::string get_exception_message( ) const {
			std::string result;
			try {
				if( m_exception ) {
					std::rethrow_exception( m_exception );
				}
			} catch( std::exception const & e ) {
				result = e.what( );
			}
			return result;
		}

		bool has_exception( ) const noexcept {
			return ExpectedSource::exception == m_source;
		}

			template<class FunctionType>
		static Expected from_code( FunctionType func ) {
			try {
				return Expected( func( ) );
			} catch( ... ) {
				return from_exception( );
			}
		}
	};	// class Expected
}	// namespace daw

static_assert(daw::traits::is_regular<daw::Expected<int>>::value, "Excepted isn't regular");
