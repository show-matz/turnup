//------------------------------------------------------------------------------
//
// LineType.cxx
//
//------------------------------------------------------------------------------
#include "LineType.hxx"

#include "TextSpan.hxx"
#include <algorithm>

namespace turnup {

    LineType GetLineType( const TextSpan* pLine ) {
        TextSpan line = pLine->Trim();
        TextSpan tmp1;
        TextSpan tmp2;
        if( line.IsEmpty() )
            return LineType::EMPTY;
        if( line[0] == '#' )
            return LineType::HEADER;
        if( (line[0] == '*' || line[0] == '+' || line[0] == '-' ) &&
                              (line[1] == ' ' || line[1] == 0x09) ) {
            return LineType::DOT_LIST;
        }
        if( line.IsMatch( "", tmp1, ". ", tmp2, "" ) ) {
            if( std::all_of( tmp1.Top(), tmp1.End(),
                             []( char c ) -> bool { return '0' <= c && c <= '9'; } ) )
                return LineType::NUM_LIST;
        }
        if( line.BeginWith( "```" ) || line.BeginWith( "~~~" ) )
            return LineType::PRE_BLOCK;
        if( line[0] == '|' )
            return LineType::TABLE;
        if( line[0] == '>' )
            return LineType::BLOCKQUOTE;
        if( line.IsMatch( "*[", tmp1, "]:", tmp2, "" ) )
            return LineType::TERM_DEF;
        if( line.BeginWith( "<!--" ) )
            return LineType::COMMENT;
        if( line.BeginWith( "---" ) || line.BeginWith( "===" )
                                    || line.BeginWith( "___" ) || line.BeginWith( "***" ) )
            return LineType::HR_LINE;
        if( line.BeginWith( "![" ) )    // embed image : 不十分だけどめんどいので
            return LineType::IMAGE;

        return LineType::PARAGRAPH;
    }

} // namespace turnup

