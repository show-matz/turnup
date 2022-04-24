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
		if( line.BeginWith( "![" ) )	// embed image : 不十分だけどめんどいので
			return LineType::IMAGE;

		return LineType::PARAGRAPH;
	}

//	static bool IsTopOfNormalList( const char* pTop, const char* pEnd ) {
//		//行頭が *,+,- のいずれかでスペースまたはタブが後続していれば順序なしリスト
//		if( pTop == pEnd )
//			return false;
//		if( *pTop != '*' && *pTop != '+' && *pTop != '-')
//			return false;
//		if( ++pTop == pEnd )
//			return false;
//		if( !( *pTop == ' ' || *pTop == 0x09 ) )
//			return false;
//		//後続するのはスペースなら３文字、タブなら１文字だけ
//		TextSpan tmp{ pTop, pEnd };
//		uint32_t max = *pTop == ' ' ? 3 : 1;
//		uint32_t cnt = tmp.CountTopOf( *pTop );
//		if( max < cnt )
//			return false;
//		tmp.Chomp( cnt, 0 );
//		pTop = tmp.Top();
//		//その後ろに別の空白類文字が後続するのも（一応）ダメ
//		if( pTop < pEnd && (*pTop == ' ' || *pTop == 0x09) )
//			return false;
//		//ここに到達すれば順序なしリストと認める（つまり内容は無しでも良い）
//		return true;
//	}
//
//	static bool IsTopOfNumberedList( const char* pTop, const char* pEnd ) {
//		//行頭が 1 以上の連続する数字で、ピリオド、スペースまたはタブが後続していれば順序付きリスト
//		if( pTop == pEnd )
//			return false;
//		uint32_t cnt = 0;
//		for( ; '0' <= *pTop && *pTop <= '9'; ++pTop )
//			++cnt;
//		if( !cnt || *pTop != '.' )
//			return false;
//		if( ++pTop == pEnd )
//			return false;
//		if( !( *pTop == ' ' || *pTop == 0x09 ) )
//			return false;
//		//後続するのはスペースなら３文字、タブなら１文字だけ
//		TextSpan tmp{ pTop, pEnd };
//		uint32_t max = *pTop == ' ' ? 3 : 1;
//		cnt = tmp.CountTopOf( *pTop );
//		if( max < cnt )
//			return false;
//		tmp.Chomp( cnt, 0 );
//		pTop = tmp.Top();
//		//その後ろに別の空白類文字が後続するのも（一応）ダメ
//		if( pTop < pEnd && (*pTop == ' ' || *pTop == 0x09) )
//			return false;
//		//ここに到達すれば順序なしリストと認める（つまり内容は無しでも良い）
//		return true;
//	}

} // namespace turnup

