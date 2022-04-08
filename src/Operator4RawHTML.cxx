//------------------------------------------------------------------------------
//
// Operator4RawHTML.cxx
//
//------------------------------------------------------------------------------
#include "Operator4RawHTML.hxx"

#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4RawHTML( const TextSpan* pTop,
									  const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)docInfo;
		if( pTop->TrimTail().IsEqual( "<raw_html>" ) == false )
			return pTop;

		const TextSpan* pLine = ++pTop;
		for( ; pLine < pEnd; ++pLine ) {
			if( pLine->TrimTail().IsEqual( "</raw_html>" ) )
				break;
		}
		if( pLine == pEnd ) {
			std::cerr << "ERROR : </raw_html> is missing." << std::endl;
			return pEnd;
		}
		
		std::cout << "<!-- start raw html -->" << std::endl;
		for( ; pTop < pLine; ++pTop ) {
			// TextSpan::WriteTo() は文字エスケープなどをするのでここでは避ける
			std::cout.write( pTop->Top(), pTop->ByteLength() );
			std::cout << std::endl;
		}
		std::cout << "<!-- end raw html -->" << std::endl;

		// この機能は obsolete となったので警告を出しておく
		std::cerr << "WARNING : <raw_html> is obsolete. use internal filter 'raw'." << std::endl;

		return pLine + 1;
	}


} // namespace turnup

