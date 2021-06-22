//------------------------------------------------------------------------------
//
// Operator4Comment.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Comment.hxx"

#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4Comment( const TextSpan* pTop,
									  const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)docInfo;
		TextSpan tmp = pTop->Trim();
		if( tmp.BeginWith( "<!--" ) == false )
			return nullptr;
		while( pTop < pEnd ) {
			// TextSpan オブジェクトを ostream に渡すと文字エスケープなどをするのでここでは避ける
			std::cout.write( pTop->Top(), pTop->ByteLength() );
			std::cout << std::endl;
			if( tmp.EndWith( "-->" ) )
				return pTop + 1;
			++pTop;
			tmp = pTop->Trim();
		}
		return pEnd;
	}


} // namespace turnup

