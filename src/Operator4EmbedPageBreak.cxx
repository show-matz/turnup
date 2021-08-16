//------------------------------------------------------------------------------
//
// Operator4EmbedPageBreak.cxx
//
//------------------------------------------------------------------------------
#include "Operator4EmbedPageBreak.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4EmbedPageBreak( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		(void)docInfo;
		// 対象外の行であれば無視
		if( pTop->Trim().IsEqual( "<!-- embed:pagebreak -->" ) == false )
			return nullptr;
		std::cout << "<p class='pagebreak'></p>" << std::endl;
		return pTop + 1;
	}

} // namespace turnup

