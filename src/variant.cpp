// The MIT License (MIT)
//
// Copyright (c) 2013-2016 Darrell Wright
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

#include <daw/daw_exception.h>
#include <daw/daw_string.h>
#include <daw/daw_newhelper.h>
#include <daw/daw_operators.h>
#include <daw/daw_variant.h>

#include "string_helpers.h"
#include "variant.h"

using daw::string::string_join;
using namespace daw::exception;

namespace boost { namespace posix_time {
		std::string to_string( boost::posix_time::ptime const & ts ) {
			return daw::string::ptime_to_string( ts, "%Y-%m-%d %H:%M:%S %Z", "" );
		}
	}
}
namespace daw {
	namespace data {
		Variant::Variant( ): 
			m_type{ DataCellType::empty_string }, 
			m_value{ } { }

		Variant::Variant( Variant && value ) noexcept: 
				m_type{ std::move( value.m_type ) },
				m_value{ std::move( value.m_value ) } {

			if( DataCellType::string == m_type ) {
				get<daw::cstring>(value.m_value) = nullptr;
				value.m_type = DataCellType::empty_string;
			}
		}

		Variant::Variant( Variant const & value ):
					m_type{ value.m_type },
					m_value{ value.m_value } { }

		Variant & Variant::operator=( Variant const & rhs ) {
			if( this != &rhs ) {
				Variant tmp{ rhs };
				tmp.swap( *this );
			}
			return *this;
		}

		Variant & Variant::operator=( Variant && rhs ) noexcept {
			if( this != &rhs ) {
				Variant tmp{ std::move( rhs ) };
				tmp.swap( *this );
			}
			return *this;
		}

		void Variant::swap( Variant & rhs ) noexcept {
			using std::swap;
			swap( m_type, rhs.m_type );
			swap( m_value, rhs.m_value );
		}

		Variant::~Variant( ) noexcept {
			if( m_value && DataCellType::string == m_type && get<daw::cstring>(m_value).is_null( ) ) {
				try {
					m_value.reset( );
					m_type = DataCellType::empty_string;
				} catch( ... ) { /* ignore */ }
			}
		}

		Variant::Variant( integer_t value ):
			m_type{ DataCellType::integer }, 
			m_value{ std::move( value ) } { }

		Variant::Variant( real_t value ): 
			m_type{ DataCellType::real },
			m_value{ std::move( value ) } { }

		Variant::Variant( timestamp_t value ):
			m_type{ DataCellType::timestamp },
			m_value{ std::move( value ) } { }

		namespace {
			daw::cstring copy_when_needed( daw::cstring value ) {
				if( !value.is_local_string( ) ) {
					return daw::cstring( value.get( ), true, value.size( ) );
				} else {
					return value;
				}
			}
		}

		Variant::Variant( daw::cstring value ) : 
			m_type{ value.is_null( ) ? DataCellType::empty_string : DataCellType::string },
			m_value{ copy_when_needed( std::move( value ) ) } { };

		integer_t const & Variant::integer( ) const {
			dbg_throw_on_false( DataCellType::integer == m_type, "{0}: Attempt to extract an integer from a non-integer", __func__ );
			return get<integer_t>(m_value);
		}

		real_t const & Variant::real( ) const {
			dbg_throw_on_false( DataCellType::real == m_type, "{0}: Attempt to extract an real from a non-real", __func__ );
			return get<real_t>(m_value);
		}

		timestamp_t const & Variant::timestamp( ) const {
			dbg_throw_on_false( DataCellType::timestamp == m_type, "{0}: Attempt to extract an timestamp from a non-timestamp", __func__ );
			return get<timestamp_t>( m_value );
		}

		std::string Variant::string( std::string locale ) const {
			switch( m_type ) {
				case DataCellType::integer:
					return boost::lexical_cast<std::string>(integer( ));
				case DataCellType::real:
					return boost::lexical_cast<std::string>(real( ));
				case DataCellType::timestamp:
					return daw::string::ptime_to_string( timestamp( ), "%Y-%m-%d %H:%M:%S %Z", locale );
				case DataCellType::empty_string:
					return "";
				case DataCellType::string:
					dbg_throw_on_true<NullPtrAccessException>( get<daw::cstring>(m_value).is_null( ), "{0}: m_value is NULL and m_type is not an empty string. This should never happen", __func__ );
					return get<daw::cstring>(m_value).to_string( );
			}
			throw AssertException( string_join( __func__, ": Unexpected control path taken.  This should never happen" ) );
		}

		bool Variant::empty( ) const noexcept {
			return !m_value || DataCellType::empty_string == m_type || (DataCellType::string == m_type && get<daw::cstring>(m_value).is_null( ));
		}

		DataCellType Variant::type( ) const noexcept {
			if( DataCellType::empty_string == m_type ) {
				return DataCellType::string;
			}
			return m_type;
		}

		namespace impl {
			template<typename ValueType>
				inline int compare( const ValueType& lhs, const ValueType& rhs ) noexcept {
					return lhs > rhs ? 1 : lhs < rhs ? -1 : 0;
				}

			template<>
				inline int compare<std::string>( const std::string& lhs, const std::string& rhs ) noexcept {
					return lhs.compare( rhs );
				}
		}	// namespace impl

		int Variant::compare( Variant const & lhs, Variant const & rhs ) {
			dbg_throw_on_false( lhs.type( ) == rhs.type( ), "{0}: Can only compare like Variant types", __func__ );
			switch( lhs.type( ) ) {
				case DataCellType::empty_string:
					return 0;
				case DataCellType::string:
					return impl::compare( lhs.string( ), rhs.string( ) );
				case DataCellType::integer:
					return impl::compare( lhs.integer( ), rhs.integer( ) );
				case DataCellType::real:
					return impl::compare( lhs.real( ), rhs.real( ) );
				case DataCellType::timestamp:
					return impl::compare( lhs.timestamp( ), rhs.timestamp( ) );
				default:
					throw NotImplemented( string_join( __func__, ": Compare for datatype not implemented" ) );
			}
		}

		int Variant::compare( Variant const & rhs ) const {
			return Variant::compare( *this, rhs );
		}

		create_comparison_operators( Variant );

		void swap( Variant & lhs, Variant & rhs ) noexcept {
			lhs.swap( rhs );
		}
	}	// namespace data
}	// namespace daw
