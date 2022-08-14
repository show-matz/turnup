//------------------------------------------------------------------------------
//
// Operator4Header.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Header.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "Config.hxx"
#include "ToC.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

	const TextSpan* Operator4Header( const TextSpan* pTop,
									 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		auto& toc = docInfo.Get<ToC>();
		auto& styles = docInfo.Get<StyleStack>();
		uint32_t level = pTop->CountTopOf( '#' );
		if( !level || 6 < level || (*pTop)[level] != ' ' )
			return pTop;
		TextSpan tmp = TextSpan{ pTop->Top() + level, pTop->End() }.Trim();
		const char* pTag = toc.GetAnchorTag( ToC::EntryT::HEADER, tmp.Top(), tmp.End() );
		char tag[3] = { 'h', (char)('0'+level), 0 };
		styles.WriteOpenTag( std::cout, tag );
		if( pTag ) {
			std::cout << "<a name='" << pTag << "'></a>";
			docInfo.SetCurrentHeader( pTag );
		} else {
			//ToDo : error message...
		}
		auto& cfg = docInfo.Get<Config>();
		if( cfg.bNumberingHeader ) {
			char chapter[32];
			toc.GetEntryNumber( chapter, ToC::EntryT::HEADER,
										 cfg, tmp.Top(), tmp.End() );
			std::cout << chapter;
		}
		tmp.WriteTo( std::cout, docInfo,
					 cfg.bTermLinkInHeader ) << "</h" << level << ">" << std::endl;
		return pTop + 1;
	}


} // namespace turnup

