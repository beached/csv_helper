#pragma once

#ifdef _MSC_VER
#	if _MSC_VER < 1800
#		error Only Visual C++ 2013 and greater is supported
#	elif _MSC_VER == 1800
#		ifndef noexcept
#			define noexcept throw( )
#		endif
#	endif
#endif
