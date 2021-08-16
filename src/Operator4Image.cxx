//------------------------------------------------------------------------------
//
// Operator4Image.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Image.hxx"

#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4Image( const TextSpan* pTop,
									const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		TextSpan line = pTop->Trim();
		TextSpan alt;
		TextSpan url;
		if( line.IsMatch( "![", alt, "](", url, ")" ) == false )
			return nullptr;

		std::cout << "<p align='center'><img src='";
		std::cout.write( url.Top(), url.ByteLength() );
		std::cout << "' ";
		if( alt.IsEmpty() == false ) {
			std::cout << "alt='";
			alt.WriteTo( std::cout, docInfo );
			std::cout << "' ";
		}
		std::cout << "/></p>" << std::endl;
		return pTop + 1;
	}


} // namespace turnup

