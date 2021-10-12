//------------------------------------------------------------------------------
//
// Operator4Image.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Image.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
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
			return pTop;

		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( std::cout, "img",
							 " style='display: block; margin: auto;'", " src='" );
		std::cout.write( url.Top(), url.ByteLength() );
		std::cout << "' ";
		if( alt.IsEmpty() == false ) {
			std::cout << "alt='";
			alt.WriteTo( std::cout, docInfo );
			std::cout << "' ";
		}
		std::cout << "/>" << std::endl;
		return pTop + 1;
	}


} // namespace turnup

