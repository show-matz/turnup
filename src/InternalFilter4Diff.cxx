//------------------------------------------------------------------------------
//
// InternalFilter4Diff.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Diff.hxx"

#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

	bool InternalFilter4Diff( std::ostream& os,
							  const TextSpan* pTop, const TextSpan* pEnd ) {
		os << "<pre class='diff'>" << std::endl;
		for( ; pTop < pEnd; ++pTop ) {
			if( pTop->BeginWith( "---" ) || pTop->BeginWith( "+++" ) ) {
				os << "<span class='diff_head'>";
				pTop->WriteSimple( os ) << "</span>" << std::endl;
			} else if( pTop->BeginWith( "@@" )  ) {
				os << "<span class='diff_line'>";
				pTop->WriteSimple( os ) << "</span>" << std::endl;
			} else if( pTop->BeginWith( "-" ) ) {
				os << "<span class='diff_del'>";
				pTop->WriteSimple( os ) << "</span>" << std::endl;
			} else if( pTop->BeginWith( "+" ) ) {
				os << "<span class='diff_add'>";
				pTop->WriteSimple( os ) << "</span>" << std::endl;
			} else {
				pTop->WriteSimple( os ) << std::endl;
			}
		}
		os << "</pre>" << std::endl;
		return true;
	}

} // namespace turnup
