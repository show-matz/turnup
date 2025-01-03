//------------------------------------------------------------------------------
//
// Operator4Quote.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Quote.hxx"

#include "LineType.hxx"
#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "Operators.hxx"

#include <stdint.h>
#include <iostream>
#include <vector>
#include <algorithm>

namespace turnup {

    static const TextSpan* RecursiveWriteBlock( const TextSpan* pTop,
                                                const TextSpan* pEnd, DocumentInfo& docInfo );
    static const TextSpan* GetEndOfQuoteBlock( const TextSpan* pTop, const TextSpan* pEnd );
    static TextSpan RemoveQuote( const TextSpan& line );

    //------------------------------------------------------------------------------
    //
    // exported function
    //
    //------------------------------------------------------------------------------
    const TextSpan* Operator4Quote( const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo ) {
        // 現在行が引用ブロック開始か確認し、違えば終了
        LineType lineType = GetLineType( pTop );
        if( lineType != LineType::BLOCKQUOTE )
            return pTop;
        // あとは再帰関数にまかせる
        return RecursiveWriteBlock( pTop, pEnd, docInfo );
    }

    //------------------------------------------------------------------------------
    //
    // internal functions
    //
    //------------------------------------------------------------------------------
    static const TextSpan* RecursiveWriteBlock( const TextSpan* pTop,
                                                const TextSpan* pEnd, DocumentInfo& docInfo ) {
        auto& styles = docInfo.Get<StyleStack>();
        // 開始タグを出力
        styles.WriteOpenTag( std::cout, "blockquote" ) << std::endl;

        // 現在の引用ブロックの終端を検索
        const TextSpan* pTmp = GetEndOfQuoteBlock( pTop, pEnd );

        // ブロック範囲に対して、引用除去をしてから出力
        std::vector<TextSpan> lines;
        std::transform( pTop, pTmp, std::back_inserter( lines ),
                        []( const TextSpan& line ) -> TextSpan {
                            return RemoveQuote( line );
                        } );
        const TextSpan* p1 = &(lines[0]);
        const TextSpan* p2 = p1 + lines.size();
        Operators operators;
        while( p1 < p2 ) {
            p1 = operators.OperateLines( p1, p2, docInfo );
        }
        // ul/ol の終了タグを出力
        std::cout << "</blockquote>" << std::endl;
        return pTmp;
    }

    static const TextSpan* GetEndOfQuoteBlock( const TextSpan* pTop, const TextSpan* pEnd ) {
        for( auto pLine = pTop; pLine < pEnd; ++pLine ) {
            LineType lineType = GetLineType( pLine );
            if( lineType != LineType::BLOCKQUOTE )
                return pLine;
        }
        return pEnd;
    }

    static TextSpan RemoveQuote( const TextSpan& line ) {
        TextSpan tmp = line.TrimHead();
        const char* pTop = tmp.Top();
        const char* pEnd = tmp.End();
        if( *pTop == '>' )
            ++pTop;
        if( *pTop == ' ' )
            ++pTop;
        return TextSpan{ pTop, pEnd };
    }

} // namespace turnup

