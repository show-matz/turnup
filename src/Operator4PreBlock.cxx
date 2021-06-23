//------------------------------------------------------------------------------
//
// Operator4PreBlock.cxx
//
//------------------------------------------------------------------------------
#include "Operator4PreBlock.hxx"

#include "TextSpan.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

	typedef void CodeWriteFunc( std::ostream& os,
								const TextSpan* pTop, const TextSpan* pEnd );

	static bool IsStartOfPreBlock( const TextSpan& line,
								   TextSpan& endTag, TextSpan& type );
	static const TextSpan* FindEndOfBlock( const TextSpan* pTop,
										   const TextSpan* pEnd, const TextSpan& type );
	static void DefaultCodeWriter( std::ostream& os,
								   const TextSpan* pTop, const TextSpan* pEnd );
	static CodeWriteFunc* FindCodeWriter( const TextSpan& type );


	const TextSpan* Operator4PreBlock( const TextSpan* pTop,
									   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)docInfo;
		TextSpan endTag;
		TextSpan type;
		if( IsStartOfPreBlock( *pTop, endTag, type ) == false )
			return nullptr;
		const TextSpan* pBlockEnd = FindEndOfBlock( ++pTop, pEnd, endTag );
		auto pWriter = FindCodeWriter( type );
		pWriter( std::cout, pTop, pBlockEnd );
		return pBlockEnd + 1;
	}


	static bool IsStartOfPreBlock( const TextSpan& line,
								   TextSpan& endTag, TextSpan& type ) {
		auto p1 = line.Top();
		auto p2 = line.Top();
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

	static void DefaultCodeWriter( std::ostream& os,
								   const TextSpan* pTop, const TextSpan* pEnd ) {
		os << "<pre>" << std::endl;
		for( ; pTop < pEnd; ++pTop ) {
			pTop->WriteSimple( os ) << std::endl;
		}
		os << "</pre>" << std::endl;
	}

	static CodeWriteFunc* FindCodeWriter( const TextSpan& type ) {
		(void)type;
		return DefaultCodeWriter;
	}

} // namespace turnup

