//------------------------------------------------------------------------------
//
// Operator4PreBlock.cxx
//
//------------------------------------------------------------------------------
#include "Operator4PreBlock.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "Filters.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

	static bool IsStartOfPreBlock( const TextSpan& line,
								   TextSpan& endTag, TextSpan& type );
	static const TextSpan* FindEndOfBlock( const TextSpan* pTop,
										   const TextSpan* pEnd, const TextSpan& type );


	const TextSpan* Operator4PreBlock( const TextSpan* pTop,
									   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)docInfo;
		TextSpan endTag;
		TextSpan type;
		if( IsStartOfPreBlock( *pTop, endTag, type ) == false )
			return pTop;
		const TextSpan* pBlockEnd = FindEndOfBlock( ++pTop, pEnd, endTag );
		auto& filters = docInfo.Get<Filters>();
		bool ret = filters.ExecuteFilter( std::cout, type, docInfo, pTop, pBlockEnd );
		if( !ret ) {
			//ToDo : invalid 'type' : error message...?
		}
		return pBlockEnd + 1;
	}


	static bool IsStartOfPreBlock( const TextSpan& line,
								   TextSpan& endTag, TextSpan& type ) {
		auto p1 = line.Top();
		auto p2 = line.End();
		if( !!::strncmp( p1, "```", 3 ) && !!::strncmp( p1, "~~~", 3 ) )
			return false;
		endTag = TextSpan{ p1, p1 + 3 };
		type   = TextSpan{ p1 + 3, p2 }.Trim();
		return true;
	}

	static const TextSpan* FindEndOfBlock( const TextSpan* pTop,
										   const TextSpan* pEnd, const TextSpan& type ) {
		auto pType = type.Top();
		auto len   = type.ByteLength();
		for( ; pTop < pEnd; ++pTop ) {
			if( pTop->ByteLength() == len && !::strncmp( pTop->Top(), pType, len ) )
				return pTop;
		}
		return pEnd;
	}

} // namespace turnup

