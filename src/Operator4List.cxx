//------------------------------------------------------------------------------
//
// Operator4List.cxx
//
//------------------------------------------------------------------------------
#include "Operator4List.hxx"

#include "LineType.hxx"
#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "Operators.hxx"

#include <stdint.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string.h>
#include <assert.h>

namespace turnup {

	static const TextSpan* RecursiveWriteBlock( const TextSpan* pTop,
												const TextSpan* pEnd, 
												LineType lineType, DocumentInfo& docInfo );
	static const TextSpan* GetEndOfCurrentItem( const TextSpan* pTop,
												const TextSpan* pEnd, const TextSpan*& pEnd1st );
	static TextSpan SkipListHead( const TextSpan* pLine );
	static TextSpan RemoveIndent( const TextSpan& line );
	static void WriteListEntry( LineType lineType, const TextSpan* pTop, 
								const TextSpan* pMid, const TextSpan* pEnd, DocumentInfo& docInfo );
	static bool IsCheckListItem( LineType lineType, TextSpan& line, bool& checked );

	//------------------------------------------------------------------------------
	//
	// inline function
	//
	//------------------------------------------------------------------------------
	inline bool IsIndentedLine( const TextSpan& line ) {
		return line[0] == 0x09 || line[0] == ' ';
	}

	//------------------------------------------------------------------------------
	//
	// exported function
	//
	//------------------------------------------------------------------------------
	const TextSpan* Operator4List( const TextSpan* pTop,
								   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		// 現在行がリスト開始か確認し、違えば終了
		LineType lineType = GetLineType( pTop );
		if( lineType != LineType::DOT_LIST && lineType != LineType::NUM_LIST )
			return pTop;
		// あとは再帰関数にまかせる
		return RecursiveWriteBlock( pTop, pEnd, lineType, docInfo );
	}

	//------------------------------------------------------------------------------
	//
	// internal functions
	//
	//------------------------------------------------------------------------------
	static const TextSpan* RecursiveWriteBlock( const TextSpan* pTop,
												const TextSpan* pEnd, 
												LineType lineType, DocumentInfo& docInfo ) {
		const char* const TAG = (lineType == LineType::DOT_LIST) ? "ul" : "ol";

		auto& styles = docInfo.Get<StyleStack>();
		// ul/ol の開始タグを出力
		styles.WriteOpenTag( std::cout, TAG ) << std::endl;

		// 同一のリスト種別の要素が続く限りループ
		while( pTop < pEnd ) {
			// 現在のリスト要素の終端を検索
			const TextSpan* pEnd1st = nullptr;
			const TextSpan* pTmp    = GetEndOfCurrentItem( pTop, pEnd, pEnd1st );

			// 単一行（これがほとんどのはず）の場合、シンプルに出力
			if( (pTmp - pTop) == 1 ) {
				TextSpan line = SkipListHead( pTop );
				bool checked = false;
				if( IsCheckListItem( lineType, line, checked ) == false )
					styles.WriteOpenTag( std::cout, "li" );
				else
					std::cout << "<li style='list-style-type:none;'>"	//MEMO : ignore StyleStack.
							  << "<input type='checkbox' onclick='return false;'"
							  << (checked ? " checked" : "") << ">";
				line.WriteTo( std::cout, docInfo );
				std::cout << "</li>" << std::endl;

			// 複数行の場合、インデント除去をしてから出力
			} else {
				std::vector<TextSpan> lines;
				std::transform( pTop, pTmp, std::back_inserter( lines ),
								[]( const TextSpan& line ) -> TextSpan {
									return RemoveIndent( line );
								} );
				const TextSpan* p = &(lines[0]);
				WriteListEntry( lineType, p, p + (pEnd1st - pTop), p + lines.size(), docInfo );
			}
			// 次のリスト要素（ではないかもしれない）の種別を確認し、違えば脱出
			LineType nextType = GetLineType( pTmp );
			pTop = pTmp;
			if( nextType != lineType )
				break;
		}
		// ul/ol の終了タグを出力
		std::cout << "</" << TAG << ">" << std::endl;
		return pTop;
	}

	//MEMO : 現在のリストアイテムの行範囲を調べ、終端行を返す。同時に、先頭部分の終端行を pEnd1st にセットする
	static const TextSpan* GetEndOfCurrentItem( const TextSpan* pTop,
												const TextSpan* pEnd, const TextSpan*& pEnd1st ) {

		// リストの先頭行はひとまず LineType::PARAGRAPH としておく
		LineType lineType = LineType::PARAGRAPH;

		// 次の行から終端を探すループを開始
		auto pNext = pTop + 1;
		pEnd1st = pNext;
		for( bool b1st = true; pNext < pEnd; ++pNext ) {
			TextSpan line = *pNext;
			// インデント有無をチェックし、されてるならインデント除去してから行タイプを取得
			bool bIndented = IsIndentedLine( line );
			if( bIndented )
				line = RemoveIndent( line );
			LineType nextType = GetLineType( pNext );

			// インデントされている場合
			if( bIndented ) {
				if( nextType == LineType::PARAGRAPH ) {
					if( b1st )
						pEnd1st = pNext + 1;
				} else {
					b1st = false;
				}
			// インデントされていない場合
			} else {
				// PARAGRAPH の行継続の場合はまだ続行
				if( lineType == LineType::PARAGRAPH && lineType == nextType ) {
					if( b1st )
						pEnd1st = pNext + 1;
				// EMPTY の場合はまだ続行
				} else if( nextType == LineType::EMPTY ) {
					if( b1st )
						pEnd1st = pNext;
					b1st     = false;
				// 上記以外の場合、そこがリストアイテムの終端 ⇒ 脱出
				} else {
					break;
				}
			}
			lineType = nextType;
		}
		// ここで先頭部分に後続するとされた範囲が全て空行なら範囲を詰める
		if( std::all_of( pEnd1st, pNext,
						 []( const TextSpan& line ) -> bool {
							 return GetLineType( &line ) == LineType::EMPTY; } ) )
			pNext = pEnd1st;
		return pNext;
	}

	static TextSpan SkipListHead( const TextSpan* pLine ) {
		const char* pTop = pLine->Top();
		const char* pEnd = pLine->End();
		if( *pTop == '*' || *pTop == '+' || *pTop == '-' )
			++pTop;
		else {
			while( '0' <= *pTop && *pTop <= '9' )
				++pTop;
			if( *pTop == '.' )
				++pTop;
		}
		while( pTop < pEnd && (*pTop == ' ' || *pTop == 0x09) )
			++pTop;
		return TextSpan{ pTop, pEnd };
	}

	static TextSpan RemoveIndent( const TextSpan& line ) {
		const char* p1 = line.Top();
		const char* p2 = line.End();
		// 先頭がタブ文字だったら単純に除去
		if( p1 < p2 && *p1 == 0x09 )
			return TextSpan{ p1 + 1, p2 };
		// それ以外の場合、先頭のスペース文字を最大４文字除去
		else {
			uint32_t cnt = line.CountTopOf( ' ' );
			return TextSpan{ p1 + std::min<uint32_t>( cnt, 4 ), p2 };
		}
	}

	static void WriteListEntry( LineType lineType, const TextSpan* pTop, 
								const TextSpan* pMid, const TextSpan* pEnd, DocumentInfo& docInfo ) {

		//MEMO : これは複数行のリストエントリを出力する
		//MEMO : RemoveIndent によってインデントが除去された行シーケンスを受け取る
		assert( 1 < (pEnd - pTop) );

		// <li> タグを出力（でも改行はひとまずしない）
		auto& styles = docInfo.Get<StyleStack>();
		TextSpan firstLine = SkipListHead( pTop );
		bool checked = false;
		if( IsCheckListItem( lineType, firstLine, checked ) == false )
			styles.WriteOpenTag( std::cout, "li" );
		else
			std::cout << "<li style='list-style-type:none;'>"	//MEMO : ignore StyleStack.
					  << "<input type='checkbox' onclick='return false;'"
					  << (checked ? " checked" : "") << ">";

		//最初のパラグラフ？を出力
		if( pMid == pTop + 1 ) {
			//１行だけの場合、その行をシンプルに出力して改行
			firstLine.WriteTo( std::cout, docInfo ) << std::endl;;
			pTop = pMid;
		} else {
			//複数行の場合、<p> - </p> で括って出力
			styles.WriteOpenTag( std::cout, "p" );
			firstLine.WriteTo( std::cout, docInfo ) << std::endl;;
			for( ++pTop; pTop < pMid; ++pTop )
				pTop->WriteTo( std::cout, docInfo ) << std::endl;;
			std::cout << "</p>" << std::endl;
		}

		//残りの行を処理
		Operators operators;
		while( pTop < pEnd ) {
			pTop = operators.OperateLines( pTop, pEnd, docInfo );
		}
		// </li> タグを出力
		std::cout << "</li>" << std::endl;
	}

	static bool IsCheckListItem( LineType lineType, TextSpan& line, bool& checked ) {
		if( lineType != LineType::DOT_LIST )
			return false;
		auto p = line.Top();
		if( p[0] != '[' || p[2] != ']' || (p[3] != ' ' && p[3] != 0x09) )
			return false;
		char c = p[1];
		if( c != ' ' && c != 'x' && c != 'X' && c != 'o' && c != 'O' && c != 'v' && c != 'V' )
			return false;
		checked = (c != ' ');
		line = line.Chomp( 3, 0 ).TrimHead();
		return true;
	}

} // namespace turnup

