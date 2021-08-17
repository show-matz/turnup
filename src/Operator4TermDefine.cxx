//------------------------------------------------------------------------------
//
// Operator4TermDefine.cxx
//
//------------------------------------------------------------------------------
#include "Operator4TermDefine.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "Glossary.hxx"
#include "Utilities.hxx"

#include <iostream>
#include <algorithm>

namespace turnup {

	bool IsTermDefine( const TextSpan& line,
					   const char*& pTop, const char*& pEnd ) {
		if( line.BeginWith( "*[" ) == false )
			return false;
		const char* p1 = line.Top();
		const char* p3 = line.End();
		const char* target = "]:";
		const char* p2 = std::search( p1 + 2, p3, target, target + 2 );
		if( p2 == p3 )
			return false;
		pTop = p1 +2;
		pEnd = p2;
		return true;
	}

	const TextSpan* Operator4TermDefine( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		const char* p1;
		const char* p2;
		if( IsTermDefine( *pTop, p1, p2 ) == false )
			return nullptr;

		{
			const char* pTermTop = p1;
			const char* pTermEnd = p2;
			Utilities::Trim( pTermTop, pTermEnd );
			auto& glossary = docInfo.Get<Glossary>();
			const char* pAnchor = glossary.GetAnchorTag( pTermTop, pTermEnd );
			std::cout << "<dl>" << std::endl;
			std::cout << "  <dt><a name='" << pAnchor <<  "'></a>";
			std::cout.write( pTermTop, pTermEnd - pTermTop );
			std::cout << "</dt>" << std::endl;
		}
		std::cout << "  <dd>"; {
			TextSpan line = *pTop;
			line = line.Chomp( (p2+2) - pTop->Top(), 0 ).Trim();
			line.WriteTo( std::cout, docInfo );
			while( ++pTop < pEnd ) {
				line = pTop->Trim();
				if( line.IsEmpty() )
					break;
				line.WriteTo( std::cout, docInfo );
			}
		}
		std::cout << "</dd>" << std::endl;
		std::cout << "</dl>" << std::endl;
		return pTop;
	}


} // namespace turnup

