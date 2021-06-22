//------------------------------------------------------------------------------
//
// Operator4NumberedList.cxx
//
//------------------------------------------------------------------------------
#include "Operator4NumberedList.hxx"

#include "TextSpan.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

	static uint32_t GetNumberedListLevel( TextSpan& line );

	const TextSpan* Operator4NumberedList( const TextSpan* pTop,
										   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		TextSpan line = *pTop;
		uint32_t curLevel = GetNumberedListLevel( line );
		if( !curLevel )
			return nullptr;

		for( uint32_t lvl = 0; lvl < curLevel; ++lvl )
			std::cout << "<ol>" << std::endl;

		do {
			std::cout << "<li>";
			line.WriteTo( std::cout, docInfo );
			std::cout << "</li>" << std::endl;
			if( ++pTop == pEnd )
				break;
			line = *pTop;
			uint32_t newLevel = GetNumberedListLevel( line );
			if( !newLevel )
				break;
			for( ; curLevel < newLevel; ++curLevel )
				std::cout << "<ol>" << std::endl;
			for( ; newLevel < curLevel; --curLevel )
				std::cout << "</ol>" << std::endl;
		} while( true );

		for( uint32_t lvl = 0; lvl < curLevel; ++lvl )
			std::cout << "</ol>" << std::endl;

		return pTop;
	}


	static uint32_t GetNumberedListLevel( TextSpan& line ) {
		uint32_t level = line.CountTopOf( 0x09 );
		auto p1 = line.Top() + level; 
		auto p2 = line.End();
		uint32_t cnt = 0;
		for( ;p1 < p2; ++p1, ++cnt ) {
			if( !('0' <= *p1 && *p1 <= '9') )
				break;
		}
		if( !cnt || p1[0] != '.' || p1[1] != ' ' )
			return 0;
		line = line.Chomp( (p1+2) - line.Top(), 0 ).TrimHead();
		return level + 1;
	}

} // namespace turnup

