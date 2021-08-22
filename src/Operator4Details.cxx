//------------------------------------------------------------------------------
//
// Operator4Details.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Details.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "Glossary.hxx"
#include "Utilities.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4Details( const TextSpan* pTop,
									  const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		TextSpan type;
		if( pTop->TrimTail().IsMatch( "<!-- collapse:", type, " -->" ) == false )
			return nullptr;

		if( type.IsEqual( "begin" ) ) {
			auto& styles = docInfo.Get<StyleStack>();
			styles.WriteOpenTag( std::cout, "details" ) << std::endl;
			styles.WriteOpenTag( std::cout, "summary" );
			pTop[1].WriteTo( std::cout, docInfo );
			std::cout << "</summary>" << std::endl;
			return pTop + 2;
		} else if( type.IsEqual( "end" ) ) {
			std::cout << "</details>" << std::endl;
			return pTop + 1;
		} else
			return nullptr;
	}


} // namespace turnup

