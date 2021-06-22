//------------------------------------------------------------------------------
//
// Operator4EmptyLine.cxx
//
//------------------------------------------------------------------------------
#include "Operator4EmptyLine.hxx"

#include "TextSpan.hxx"

namespace turnup {

	const TextSpan* Operator4EmptyLine( const TextSpan* pTop,
										const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)docInfo;
		uint32_t cnt = 0;
		for( ; pTop < pEnd; ++pTop, ++cnt ) {
			if( pTop->IsEmpty() == false )
				break;
		}
		return !cnt ? nullptr : pTop;
	}


} // namespace turnup

