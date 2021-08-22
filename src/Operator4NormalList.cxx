//------------------------------------------------------------------------------
//
// Operator4NormalList.cxx
//
//------------------------------------------------------------------------------
#include "Operator4NormalList.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

	static uint32_t GetNormalListLevel( TextSpan& line );
	static bool IsCheckListItem( TextSpan& line, bool& checked );

	const TextSpan* Operator4NormalList( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		auto& styles = docInfo.Get<StyleStack>();
		TextSpan line = *pTop;
		uint32_t curLevel = GetNormalListLevel( line );
		if( !curLevel )
			return nullptr;

		for( uint32_t lvl = 0; lvl < curLevel; ++lvl )
			styles.WriteOpenTag( std::cout, "ul" ) << std::endl;

		do {
			bool checked = false;
			if( IsCheckListItem( line, checked ) == false ) {
				styles.WriteOpenTag( std::cout, "li" );
				line.WriteTo( std::cout, docInfo );
				std::cout << "</li>" << std::endl;
			} else {
				std::cout << "<li style='list-style-type:none;'>"	//MEMO : ignore StyleStack.
						  << "<input type='checkbox' onclick='return false;'"
						  << (checked ? " checked" : "") << ">";
				line.WriteTo( std::cout, docInfo );
				std::cout << "</li>" << std::endl;
			}
			if( ++pTop == pEnd )
				break;
			line = *pTop;
			uint32_t newLevel = GetNormalListLevel( line );
			if( !newLevel )
				break;
			for( ; curLevel < newLevel; ++curLevel )
				styles.WriteOpenTag( std::cout, "ul" ) << std::endl;
			for( ; newLevel < curLevel; --curLevel )
				std::cout << "</ul>" << std::endl;
		} while( true );

		for( uint32_t lvl = 0; lvl < curLevel; ++lvl )
			std::cout << "</ul>" << std::endl;

		return pTop;
	}


	static uint32_t GetNormalListLevel( TextSpan& line ) {
		uint32_t level = line.CountTopOf( 0x09 );
		auto p1 = line.Top() + level; 
		if( (*p1 != '-' && *p1 != '+' && *p1 != '*') || p1[1] != ' ' )
			return 0;
		line = line.Chomp( (p1+1) - line.Top(), 0 ).TrimHead();
		return level + 1;
	}

	static bool IsCheckListItem( TextSpan& line, bool& checked ) {
		auto p = line.Top();
		if( p[0] != '[' || p[2] != ']' || p[3] != ' ' )
			return false;
		if( p[1] != ' ' && p[1] != 'x' && p[1] != 'X' )
			return false;
		checked = (p[1] == 'x' || p[1] != 'X');
		line = line.Chomp( 3, 0 ).TrimHead();
		return true;
	}

} // namespace turnup

