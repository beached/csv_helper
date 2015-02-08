#pragma once

#include <boost/date_time/posix_time/ptime.hpp>
#include <cstdint>

namespace daw {
	namespace data {
		using real_t = float;
		using integer_t = int32_t;
		using timestamp_t = boost::posix_time::ptime;
		enum class DataCellType: int8_t { empty_string = 0, integer = 1, real = 2, string = 3, timestamp = 4 };

		class DataCell;
		class DataColumnDense;
		class DataColumnSparse;
		class DataTable;		
	}
}
