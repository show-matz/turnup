//------------------------------------------------------------------------------
//
// InternalFilter4Raw.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Raw.hxx"

#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

	bool InternalFilter4Raw( std::ostream& os,
							 const TextSpan* pTop, const TextSpan* pEnd ) {
		for( ; pTop < pEnd; ++pTop )
			os << *pTop << std::endl;
		return true;
	}

} // namespace turnup
