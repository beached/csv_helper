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

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_array.hpp>
#include <cinttypes>

#include <daw/daw_cstring.h>
#include <daw/daw_traits.h>
#include <daw/daw_exception.h>
#include <daw/daw_variant.h>

#include "data_types.h"
#include "defs.h"

namespace boost { namespace posix_time {
		std::string to_string( boost::posix_time::ptime const & ts );
	} 
}
namespace daw {
	namespace data {
		class Variant {
			DataCellType m_type;
			using value_t = daw::variant_t<integer_t, real_t, timestamp_t, daw::cstring>;
			value_t m_value;
		public:
			Variant( );
			~Variant( );

			Variant( Variant const & other ) = default;
			Variant& operator=(Variant const & rhs) = default;

			Variant( Variant&& value ) noexcept;
			Variant& operator=(Variant && rhs) noexcept;

			explicit Variant( integer_t value );
			explicit Variant( real_t value );
			explicit Variant( timestamp_t value );
			explicit Variant( daw::cstring value );

			bool empty( ) const noexcept;
			DataCellType type( ) const noexcept;

			integer_t const & integer( ) const;
			real_t const & real( ) const;
			timestamp_t const & timestamp( ) const;

			std::string string( std::string locale = "" ) const;

			static int compare( Variant const & lhs, Variant const & rhs );
			int compare( Variant const & rhs ) const;

			void swap( Variant & rhs ) noexcept;
		};	// Variant

		bool operator==(Variant const & lhs, Variant const & rhs);
		bool operator!=(Variant const & lhs, Variant const & rhs);

		bool operator<(Variant const & lhs, Variant const & rhs);
		bool operator>(Variant const & lhs, Variant const & rhs);
		bool operator<=(Variant const & lhs, Variant const & rhs);
		bool operator>=(Variant const & lhs, Variant const & rhs);

		void swap( Variant & lhs, Variant & rhs ) noexcept;
	}	// namespace data
}	// namespace daw
