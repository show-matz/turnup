//------------------------------------------------------------------------------
//
// Operator4Quote.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Quote.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

	static uint32_t GetQuoteLevel( TextSpan& line );

	const TextSpan* Operator4Quote( const TextSpan* pTop,
									const TextSpan* pEnd, DocumentInfo& docInfo ) {
		TextSpan line = *pTop;
		uint32_t curLevel = GetQuoteLevel( line );
		if( !curLevel )
			return pTop;

		auto& styles = docInfo.Get<StyleStack>();

		for( uint32_t lvl = 0; lvl < curLevel; ++lvl )
			styles.WriteOpenTag( std::cout, "blockquote" ) << std::endl;

		do {
			line.WriteTo( std::cout, docInfo );
			styles.WriteOpenTag( std::cout, "br" ) << std::endl;
			if( ++pTop == pEnd )
				break;
			line = *pTop;
			uint32_t newLevel = GetQuoteLevel( line );
			if( !newLevel )
				break;
			for( ; curLevel < newLevel; ++curLevel )
				styles.WriteOpenTag( std::cout, "blockquote" ) << std::endl;
			for( ; newLevel < curLevel; --curLevel )
				std::cout << "</blockquote>" << std::endl;
		} while( true );

		for( uint32_t lvl = 0; lvl < curLevel; ++lvl )
			std::cout << "</blockquote>" << std::endl;

		return pTop;
	}


	static uint32_t GetQuoteLevel( TextSpan& line ) {
		//ToDo : ひとまず > の間に空白を許さない実装（改善の余地あり）
		uint32_t level = line.CountTopOf( '>' );
		line = line.Chomp( level, 0 ).TrimHead();
		return level;
	}

} // namespace turnup

