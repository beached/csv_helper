#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <string>

namespace daw { namespace string {
	/// See http://www.boost.org/doc/libs/1_55_0/doc/html/date_time/date_time_io.html for formatting info
	std::string ptime_to_string( const boost::posix_time::ptime& value, const std::string& format = "%Y-%m-%d %H:%M:%S %Z", const std::string locale_str = "" );
	std::string ptime_to_string( const boost::posix_time::time_duration& value, bool show_seconds = true );
}}
