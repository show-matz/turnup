//------------------------------------------------------------------------------
//
// Operator4FigureAndTable.cxx
//
//------------------------------------------------------------------------------
#include "Operator4FigureAndTable.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "Config.hxx"
#include "ToC.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

	const TextSpan* Operator4FigureAndTable( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		const char* const classes[] = { "", "tbl_title", "fig_title" };

		ToC::EntryT	type;
		TextSpan	title;
		if( pTop->IsMatch( "Figure.", title, "" ) )
			type = ToC::EntryT::FIGURE;
		else if ( pTop->IsMatch( "Table.", title, "" ) )
			type = ToC::EntryT::TABLE;
		else
			return nullptr;

		title = title.Trim();

		auto& toc = docInfo.Get<ToC>();
		const char* pTag = toc.GetAnchorTag( type, title.Top(), title.End() );

		std::cout << "<p class='" << classes[static_cast<uint32_t>( type )] << "'>";
		if( pTag )
			std::cout << "<a name='" << pTag << "'></a>";
		else {
			//ToDo : error message...
		}

		auto& cfg = docInfo.Get<Config>(); {
			char chapter[64];
			toc.GetEntryNumber( chapter, type, cfg, title.Top(), title.End() );
			std::cout << chapter << ' ';
		}

		title.WriteTo( std::cout, docInfo,
					   cfg.bTermLinkInHeader ) << "</p>" << std::endl;
		return pTop + 1;
	}


} // namespace turnup

