#pragma once

#include "cstring.h"
#include "data_types.h"
#include "defs.h"
#include "variant.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cinttypes>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <memory>
#include <string>
#include <unordered_map>
#include "header_libraries/workarounds.h"
#include "header_libraries/daw_traits.h"

namespace daw {
	namespace data {
		class DataCell {
		public:
			DataCell( ) = default;
			DataCell( const DataCell& value ) = default;

			DataCell( DataCell&& value ) noexcept;
			DataCell& operator=(DataCell value) noexcept;
			~DataCell( ) = default;

			explicit DataCell( cstring value );
			explicit DataCell( integer_t value );
			explicit DataCell( real_t value );
			explicit DataCell( timestamp_t value );

			explicit operator bool( ) const noexcept;

			std::string string( ) const;
			std::string to_string( std::string locale_str = "" ) const;
			DataCellType type( ) const noexcept;
			integer_t integer( ) const;
			real_t real( ) const;
			timestamp_t timestamp( ) const;
			real_t numeric( ) const;
			bool empty( ) const noexcept;

			static DataCell from_string( cstring value, std::string locale_str = "" );
			static DataCell from_time_string( std::string value, std::string format = "" );

			static const std::function<bool( DataCell const &, DataCell const & )> cmp_integer;
			static const std::function<bool( DataCell const &, DataCell const & )> cmp_real;
			static const std::function<bool( DataCell const &, DataCell const & )> cmp_timestamp;
			static const std::function<bool( DataCell const &, DataCell const & )> cmp_other;
			static const std::function<bool( DataCell const &, DataCell const & )> get_compare( const DataCell& cell );

			static int compare( const DataCell& lhs, DataCell const & );

			bool operator==(DataCell const &) const;
			bool operator<(DataCell const &) const;
			bool operator!=(DataCell const &) const;
			bool operator>(DataCell const &) const;
			bool operator<=(DataCell const &) const;
			bool operator>=(DataCell const &) const;

			// Data
			static const DataCell s_empty_cell;
		private:
			Variant m_item;
		};
		static_assert(daw::traits::is_regular<DataCell>::value, "DataCell isn't regular");

		bool is_numeric( daw::data::DataCellType const & ct );
		bool is_numeric( daw::data::DataCell const & value );
	}	// namespace data
}	// namespace daw
