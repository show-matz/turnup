//------------------------------------------------------------------------------
//
// Operator4EmbedFootnotes.cxx
//
//------------------------------------------------------------------------------
#include "Operator4EmbedFootnotes.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "Footnotes.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4EmbedFootnotes( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		// 対象外の行であれば無視
		if( pTop->Trim().IsEqual( "<!-- embed:footnotes -->" ) == false )
			return pTop;
		auto& footnotes = docInfo.Get<Footnotes>();
		footnotes.WriteFootnotes( std::cout, docInfo );
		return pTop + 1;
	}

} // namespace turnup

