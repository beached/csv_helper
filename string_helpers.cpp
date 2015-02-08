#include "string_helpers.h"
#include "header_libraries/make_unique.h"
#include "header_libraries/daw_string.h"
#include <memory>
#include <string>

namespace daw {
	namespace string {
		std::string ptime_to_string( const boost::posix_time::ptime& value, const std::string& format, const std::string locale_str ) {
			try {
				static std::stringstream ss( "" );
				static std::unique_ptr<boost::posix_time::time_facet> facet;
				if( !facet ) {
					facet = daw::make_unique<boost::posix_time::time_facet>( 1u );
					ss.imbue( std::locale( std::locale( locale_str.c_str( ) ), facet.get( ) ) );
				}
				clear( ss );
				facet->format( format.c_str( ) );
				ss << value;
				return ss.str( );
			} catch( const std::exception& ex ) {
				throw ex;
			}
		}

		std::string ptime_to_string( const boost::posix_time::time_duration& value, bool show_seconds ) {
			try {
				static std::stringstream ss( "" );
				clear( ss );
				ss << std::setw( 2 ) << std::setfill( '0' ) << value.hours( );
				ss << ":";
				ss << std::setw( 2 ) << std::setfill( '0' ) << value.minutes( );
				if( show_seconds ) {
					ss << ":";
					ss << std::setw( 2 ) << std::setfill( '0' ) << value.seconds( );
				}
				return ss.str( );
			} catch( const std::exception& ex ) {
				throw ex;
			}
		}
	}	// namespace string
}	// namespace daw
