#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>
#include <ctime>
#include "data_cell.h"
#include "string_helpers.h"
#include "exception.h"
#include "header_libraries/daw_string.h"
#include "header_libraries/daw_string.h"
#include "workarounds.h"
#include <exception>
#include <iomanip>
#include <locale>
#include <numeric>
#include <sstream>
#include <string>
#include <typeinfo>
#include <unordered_map>

namespace daw {
	namespace data {
		using namespace daw::exception;
		using daw::string::string_join;

		const DataCell DataCell::s_empty_cell = DataCell( );
		namespace {
			const std::string s_emptystring = std::string( );
			const std::string s_default_timestamp_format = "%Y-%m-%d %H:%M:%S %Z";

			DataCellType get_cell_type( cstring& value, std::string locale_str ) {
				// DAW erase

				if( value.is_null( ) ) {
					return DataCellType::empty_string;
				}
				const auto len = value.size( );
				if( 0 == len ) {
					return DataCellType::empty_string;
				}
				using charT = char;
				// Can only set locale once per application start.  It was slow
				static const std::locale loc = std::locale( locale_str );
				static const charT decimal_point = std::use_facet< std::numpunct<charT>>( loc ).decimal_point( );

				bool is_negative = false;
				bool has_decimal = false;

				size_t startpos = 0;

				if( '-' == value[startpos] ) {
					is_negative = true;
					startpos = 1;
				}

				for( size_t n = startpos; n < len; ++n ) {
					if( '-' == value[n] ) {	// We have already had a - or it is not the first
						return DataCellType::string;
					} else if( decimal_point == value[n] ) {
						if( has_decimal || len == n + 1 ) {	// more than one decimal point or we are the last entry and there is not a possibility of another number
							return DataCellType::string;
						}
						has_decimal = true;
					} else if( !std::isdigit( value[n], loc ) ) {	// Of course, not a numeral
						return DataCellType::string;
					}
					// All other items are numbers and we keep checking
				}
				if( has_decimal ) {
					return DataCellType::real;
				} else {
					return DataCellType::integer;
				}
			}
		}	// Namespace static
	}	// Namespace data

	namespace algorithm {
		namespace {
			using namespace daw::data;
			inline void convertToString( const timestamp_t& from, std::string& to, const std::string& locale_str ) {
				to = daw::string::ptime_to_string( from, s_default_timestamp_format, locale_str );
			}
		}	// Namespace static
	}	// Namespace algorithm

	namespace data {
		using namespace daw::algorithm;

		DataCell::DataCell( DataCell&& value ) noexcept: m_item( std::move( value.m_item ) ) { }

		DataCell& DataCell::operator=(DataCell rhs) {
			m_item = std::move( rhs.m_item );
			return *this;
		}

		DataCell::DataCell( cstring value ) : m_item( std::move( value ) ) { }
		DataCell::DataCell( integer_t value ) : m_item( std::move( value ) ) { }
		DataCell::DataCell( real_t value ) : m_item( std::move( value ) ) { }
		DataCell::DataCell( timestamp_t value ) : m_item( std::move( value ) ) { }

		DataCell::operator bool( ) const noexcept {
			return !empty( );
		}

			std::string DataCell::to_string( std::string locale_str ) const {
			using daw::string::convertToString;
			std::string result;
			switch( type( ) ) {
			case DataCellType::empty_string:
			case DataCellType::string:
				result = string( );
				break;
			case DataCellType::integer:
				convertToString( integer( ), result );
				break;
			case DataCellType::real:
				convertToString( real( ), result );
				break;
			case DataCellType::timestamp:
				convertToString( timestamp( ), result );
				break;
			default:
				throw daw::exception::FatalError( string_join( __func__, ": Unexpected Error" ) );
			}
			return result;
		}

		DataCellType DataCell::type( ) const noexcept {
			return m_item.type( );
		}

			integer_t DataCell::integer( ) const {
			return m_item.integer( );
		}

		real_t DataCell::real( ) const {
			return m_item.real( );
		}

		timestamp_t DataCell::timestamp( ) const {
			return m_item.timestamp( );
		}

		std::string DataCell::string( ) const {
			return m_item.string( );
		}

		real_t DataCell::numeric( ) const {
			dbg_throw_on_false( daw::data::is_numeric( type( ) ), "Tried to call numeric( ) on a non-numeric datatype" );
			if( type( ) == DataCellType::real ) {
				return real( );
			} else {
				return static_cast<real_t>(integer( ));
			}
		}

		bool DataCell::empty( ) const noexcept {
			return m_item.empty( );
		}

			DataCell DataCell::from_string( cstring value, std::string locale_str ) {
			// Check if is integer
			const auto ct = get_cell_type( value, locale_str );
			switch( ct ) {
			case DataCellType::integer: {
				return DataCell( boost::lexical_cast<integer_t>(value.get( )) );
			}
			case DataCellType::real: {
				return DataCell( boost::lexical_cast<real_t>(value.get( )) );
			}
			case DataCellType::empty_string:
				return DataCell( );
			case DataCellType::string: {
				DataCell result( std::move( value ) );
				return std::move( result );
			}
			case DataCellType::timestamp:
				throw daw::exception::NotImplemented( string_join( __func__, ": Use from_time_string( std::string, std::string ) for time/data types" ) );
			}
			// This should never happen.  It should default to string
			throw daw::exception::FatalError( string_join( __func__, ": Could not determine data type in string" ) );
		}

		/// See http://www.boost.org/doc/libs/1_55_0/doc/html/date_time/date_time_io.html for formatting info
		DataCell DataCell::from_time_string( std::string value, std::string format ) {
			if( 0 == value.size( ) ) {
				return DataCell( );
			}
			static std::stringstream ss;
			static std::unique_ptr<boost::posix_time::time_input_facet> facet;
			if( !facet ) {
				facet = std::make_unique<boost::posix_time::time_input_facet>( 1 );
				ss.imbue( std::locale( std::locale( ), facet.get( ) ) );
			}
			clear( ss );
			if( format.empty( ) ) {
				format = s_default_timestamp_format;
			}
			facet->format( format.c_str( ) );
			ss.str( value );

			timestamp_t result;
			ss >> result;
			if( result == boost::posix_time::not_a_date_time ) {
				throw std::runtime_error( string_join( __func__, ": Format conversion error in from_time_string" ) );
			}
			return DataCell( result );
		}

		const std::function<bool( const DataCell, const DataCell )> DataCell::cmp_integer = []( const DataCell A, const DataCell B ) { return A.integer( ) < B.integer( ); };
		const std::function<bool( const DataCell, const DataCell )> DataCell::cmp_real = []( const DataCell A, const DataCell B ) { return A.real( ) < B.real( ); };
		const std::function<bool( const DataCell, const DataCell )> DataCell::cmp_timestamp = []( const DataCell A, const DataCell B ) { return A.timestamp( ) < B.timestamp( ); };
		const std::function<bool( const DataCell, const DataCell )> DataCell::cmp_other = []( const DataCell A, const DataCell B ) { return A.to_string( ).compare( B.to_string( ) ) < 0;  };

		const std::function<bool( const DataCell, const DataCell )> DataCell::get_compare( const DataCell& cell ) {
			switch( cell.type( ) ) {
			case DataCellType::integer:
				return cmp_integer;
			case DataCellType::real:
				return cmp_real;
			case DataCellType::timestamp:
				return cmp_timestamp;
			case DataCellType::empty_string:
			case DataCellType::string:
			default:
				return cmp_other;
			}
		}

		int DataCell::compare( const DataCell& lhs, const DataCell& rhs ) {
			return Variant::compare( lhs.m_item, rhs.m_item );
		}

		bool DataCell::operator==(const DataCell& rhs) const {
			return compare( *this, rhs ) == 0;
		}

		bool DataCell::operator<(const DataCell& rhs) const {
			return compare( *this, rhs ) < 0;
		}

		bool DataCell::operator!=(const DataCell& rhs) const {
			return compare( *this, rhs ) != 0;
		}

		bool DataCell::operator>(const DataCell& rhs) const {
			return compare( *this, rhs ) > 0;
		}

		bool DataCell::operator<=(const DataCell& rhs) const {
			return compare( *this, rhs ) <= 0;
		}

		bool DataCell::operator>=(const DataCell& rhs) const {
			return compare( *this, rhs ) >= 0;
		}

		const bool is_numeric( const daw::data::DataCellType ct ) {
			return daw::data::DataCellType::integer == ct || daw::data::DataCellType::real == ct;
		}

		const bool is_numeric( const daw::data::DataCell& value ) {
			const auto ct = value.type( );
			return is_numeric( ct );
		}
	}	  // Namespace data
}	// namespace daw
