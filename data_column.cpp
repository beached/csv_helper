#include <algorithm>
#include <boost/utility/string_ref.hpp>
#include <vector>
#include "data_column.h"
#include "data_cell.h"

namespace daw {
	namespace data {
		void convert_column_to_timestamp( DataColumn<std::vector<DataCell>> & column, bool is_nullable, boost::string_ref format ) {
			std::transform( column.begin( ), column.end( ), column.begin( ), [format]( DataCell const & cell ) {
				if( cell.empty( ) && is_nullable ) {
					return cell;
				}
				return DataCell::from_time_string( cell.string( ), format.to_string( ) );
			} );			
		}	
	}
}

