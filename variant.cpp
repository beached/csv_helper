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

#include "variant.h"
#include "string_helpers.h"
#include "header_libraries/exception.h"
#include "header_libraries/daw_string.h"
#include "new_helper.h"

using daw::string::string_join;
using namespace daw::exception;

namespace daw {
	namespace data {
		namespace {
			const char* create_copy( const char* value, size_t length ) {
				if( nullptr == value ) {
					return value;
				}
				char* result = new_array_throw<char>( length + 1 );
				result[length] = 0;
				memcpy( result, value, length );

				return result;
			}


			boost::posix_time::ptime get_epoch( ) {
				static auto const s_epoch = boost::posix_time::time_from_string( "1970-01-01 00:00:00.000" );
				return s_epoch;
			}

			/// <summary>We expect to own the string</summary>
			const char* copy_string( const char* value ) noexcept {
				if( nullptr == value ) {
					return value;
				}
				const auto len = strlen( value );
				if( 0 == len ) {
					return nullptr;
				}

				auto result = new_array_throw<char>( len + 1 );
				memcpy( result, value, len );
				result[len] = 0;
				return result;
			}

				cstring copy_string( cstring value ) noexcept {
				return std::move( value );
			}

				uint32_t ptime_to_uint32( boost::posix_time::ptime value ) noexcept {
				const auto diff = value - get_epoch( );
				assert( 0 <= diff.total_seconds( ) );
				return static_cast<uint32_t>(diff.total_seconds( ));
			}

				boost::posix_time::ptime uint32_to_ptime( uint32_t value ) noexcept {
				return get_epoch( ) + boost::posix_time::seconds( value );
			}
		}

		variant_union_t::variant_union_t( ) noexcept: m_string( nullptr ) { }

		variant_union_t::variant_union_t( const variant_union_t& value ) : m_string( create_copy( value.m_string, std::numeric_limits<size_t>::max( ) ) ) { }

		variant_union_t& variant_union_t::operator=(const variant_union_t& value) {
			if( this != &value ) {
				m_string = create_copy( value.m_string, std::numeric_limits<size_t>::max( ) );
			}
			return *this;
		}

		variant_union_t::variant_union_t( variant_union_t&& value ) noexcept: m_string( std::move( value.m_string ) ) {
			value.m_string = nullptr;
		}

		variant_union_t& variant_union_t::operator=(variant_union_t&& value) noexcept {
			if( this != &value ) {
				m_string = std::move( value.m_string );
				value.m_string = nullptr;
			}
			return *this;
		}

			variant_union_t::variant_union_t( integer_t value ) noexcept : m_integer( std::move( value ) ) { }
		variant_union_t::variant_union_t( real_t value ) noexcept: m_real( std::move( value ) ) { }
		variant_union_t::variant_union_t( uint32_t value ) noexcept: m_timestamp( std::move( value ) ) { }
		variant_union_t::variant_union_t( cstring value ) noexcept: m_string( value.move( ) ) { }

		Variant::Variant( ) noexcept: m_type( DataCellType::empty_string ), m_value( ) { };

		Variant::Variant( Variant&& value ) noexcept: m_type( std::move( value.m_type ) ), m_value( std::move( value.m_value ) ) {
			if( DataCellType::string == m_type ) {
				value.m_value.m_string = nullptr;
				value.m_type = DataCellType::empty_string;
			}
		}

		Variant& Variant::operator=(Variant value) noexcept {
			m_type = std::move( value.m_type );
			m_value = std::move( value.m_value );
			return *this;
		}

			Variant::~Variant( ) noexcept {
			if( DataCellType::string == m_type && nullptr != m_value.m_string ) {
				try {
					auto tmp = const_cast<char*>(m_value.m_string);
					const auto len = strlen( tmp );
					memset( tmp, 0, len );
					delete[] m_value.m_string;
					m_value.m_string = nullptr;
				} catch( ... ) { /* ignore */ }
			}
		}

			Variant::Variant( integer_t value ) noexcept:m_type( DataCellType::integer ), m_value( std::move( value ) ) { }

		Variant::Variant( real_t value ) noexcept: m_type( DataCellType::real ), m_value( std::move( value ) ) { }

		Variant::Variant( timestamp_t value ) noexcept: m_type( DataCellType::timestamp ), m_value( ptime_to_uint32( value ) ) { }

		namespace {
			cstring copy_when_needed( cstring value ) {
				if( !value.is_local_string( ) ) {
					return cstring( value.get( ), true, value.size( ) );
				} else {
					return std::move( value );
				}
			}
		}

		Variant::Variant( cstring value ) noexcept : m_type( value.is_null( ) ? DataCellType::empty_string : DataCellType::string ), m_value( copy_when_needed( std::move( value ) ) ) { };

		const integer_t& Variant::integer( ) const {
			dbg_throw_on_false( DataCellType::integer == m_type, "{0}: Attempt to extract an integer from a non-integer", __func__ );
			return m_value.m_integer;
		}

		const real_t& Variant::real( ) const {
			dbg_throw_on_false( DataCellType::real == m_type, "{0}: Attempt to extract an real from a non-real", __func__ );
			return m_value.m_real;
		}

		timestamp_t Variant::timestamp( ) const {
			dbg_throw_on_false( DataCellType::timestamp == m_type, "{0}: Attempt to extract an timestamp from a non-timestamp", __func__ );
			return uint32_to_ptime( m_value.m_timestamp );
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
				dbg_throw_on_false<NullPtrAccessException>( 0 != m_value.m_string, "{0}: m_value.m_string is NULL and m_type is not an empty string. This should never happen", __func__ );
				return std::string( m_value.m_string );
			}
			throw AssertException( string_join( __func__, ": Unexpected control path taken.  This should never happen" ) );
		}

		bool Variant::empty( ) const noexcept {
			return DataCellType::empty_string == m_type || (DataCellType::string == m_type && nullptr == m_value.m_string);
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

		int Variant::compare( const Variant& lhs, const Variant& rhs ) {
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

		bool Variant::operator==(const Variant& rhs) const {
			return compare( *this, rhs ) == 0;
		}

		bool Variant::operator!=(const Variant& rhs) const {
			return compare( *this, rhs ) != 0;
		}

		bool Variant::operator<(const Variant& rhs) const {
			return compare( *this, rhs ) < 0;
		}

		bool Variant::operator>(const Variant& rhs) const {
			return compare( *this, rhs ) > 0;
		}

		bool Variant::operator<=(const Variant& rhs) const {
			return compare( *this, rhs ) <= 0;
		}

		bool Variant::operator>=(const Variant& rhs) const {
			return compare( *this, rhs ) >= 0;
		}
	}	// namespace data
}	// namespace daw
