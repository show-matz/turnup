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
		TextSpan param;
		if( pTop->Trim().IsMatch( "<!-- embed:footnotes", param, " -->" ) ) {
			auto& footnotes = docInfo.Get<Footnotes>();
			if( param.ByteLength() == 0 )
				footnotes.WriteFootnotes( std::cout, docInfo );
			else {
				param = param.Trim();
				if( param.IsQuoted() )
					param = param.Chomp( 1, 1 );
				footnotes.WriteFootnotes( param, std::cout, docInfo );
			}
			return pTop + 1;
		}
		return pTop;
	}

} // namespace turnup

