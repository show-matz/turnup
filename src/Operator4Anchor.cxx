//------------------------------------------------------------------------------
//
// Operator4Anchor.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Anchor.hxx"

#include "DocumentInfo.hxx"
#include "TextSpan.hxx"
#include "ToC.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

	const TextSpan* Operator4Anchor( const TextSpan* pTop,
									 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		TextSpan line = pTop->Trim();
		TextSpan title;
		if( line.IsMatch( "<!-- anchor:", title, " -->" ) == false )
			return pTop;

		title = title.Trim();
		auto& toc    = docInfo.Get<ToC>();
		const char* pTag = toc.GetAnchorTag( ToC::EntryT::ANCHOR, title.Top(), title.End() );
		if( pTag )
			std::cout << "<a name='" << pTag << "' />" << std::endl;
		else {
			//ToDo : error message...
		}
		return pTop + 1;
	}

} // namespace turnup

