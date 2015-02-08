#pragma once

#include "cstring.h"
#include "data_types.h"
#include "defs.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_array.hpp>
#include <cinttypes>
#include "header_libraries/daw_traits.h"
#include "header_libraries/exception.h"
#include "header_libraries/workarounds.h"
namespace daw {
	namespace data {
		/// <summary>A simple variant that contains all needed data types</summary>
#if PACK_VARIANT == 1
#pragma pack( 1 )
#endif
		union variant_union_t {
			static_assert(daw::traits::max_sizeof<integer_t, real_t, uint32_t, char*>::value == sizeof( char* ), "char* is not the largest item in variant_union_t.  This has been assumed and is bad.");
			const integer_t m_integer;
			const real_t m_real;
			const uint32_t m_timestamp;
			const char* m_string;

			explicit variant_union_t( ) noexcept;
			explicit variant_union_t( const variant_union_t& value );
			variant_union_t& operator=(const variant_union_t& value);
			explicit variant_union_t( variant_union_t&& value ) noexcept;
			variant_union_t& operator=(variant_union_t&& value) noexcept;
			~variant_union_t( ) = default;

			explicit variant_union_t( integer_t value ) noexcept;
			explicit variant_union_t( real_t value ) noexcept;
			explicit variant_union_t( uint32_t value ) noexcept;
			explicit variant_union_t( cstring value ) noexcept;
		};


		class Variant {
		public:
			Variant( ) noexcept;
			Variant& operator=(Variant rhs) noexcept;
			Variant( Variant&& value ) noexcept;
			~Variant( ) noexcept;
			Variant( const Variant& ) = default;

			explicit Variant( integer_t value ) noexcept;
			explicit Variant( real_t value ) noexcept;
			explicit Variant( timestamp_t value ) noexcept;
			explicit Variant( cstring value ) noexcept;

			//void swap( Variant& rhs );

			bool empty( ) const noexcept;
			DataCellType type( ) const noexcept;

			const integer_t& integer( ) const;
			const real_t& real( ) const;
			timestamp_t timestamp( ) const;

			std::string string( std::string locale = "" ) const;

			static int compare( const Variant& lhs, const Variant& rhs );

			bool operator==(const Variant& rhs) const;
			bool operator!=(const Variant& rhs) const;

			bool operator<(const Variant& rhs) const;
			bool operator>(const Variant& rhs) const;
			bool operator<=(const Variant& rhs) const;
			bool operator>=(const Variant& rhs) const;

		private:
			DataCellType m_type;
			variant_union_t m_value;
		};
#if PACK_VARIANT == 1
#pragma pack( )
#endif
	}	// namespace data
}	// namespace daw
