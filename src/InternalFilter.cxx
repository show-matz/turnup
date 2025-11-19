//------------------------------------------------------------------------------
//
// InternalFilter.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter.hxx"

#include "InternalFilter4CandCxx.hxx"
#include "InternalFilter4CommonLISP.hxx"
#include "InternalFilter4Diff.hxx"
#include "InternalFilter4Shell.hxx"
#include "InternalFilter4Raw.hxx"
#include "InternalFilter4Table.hxx"
#include "InternalFilter4MathJax.hxx"

#include "TextSpan.hxx"

#include <vector>
#include <algorithm>
#include <ostream>

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class HighlightSpan
    //
    //--------------------------------------------------------------------------
    class HighlightSpan {
        friend bool operator<( const HighlightSpan&, const HighlightSpan& );
    public:
        HighlightSpan() : m_span( TextSpan{} ),
                          m_pClassName( nullptr ) {
        };
        HighlightSpan( const HighlightSpan& ) = default;
        HighlightSpan( const TextSpan& span,
                       const char* pClassName ) : m_span( span ),
                                                  m_pClassName( pClassName ) {
        };
        ~HighlightSpan() {
        };
    public:
        inline const char* Top() const { return m_span.Top(); }
        inline const char* End() const { return m_span.End(); }
        inline const TextSpan& Span() const { return m_span; }
        inline const char* Class() const { return m_pClassName; }
        inline bool IsOverlap( const HighlightSpan& other ) const {
            if( other.End() <= this->Top() || this->End() <= other.Top() )
                return false;
            return true;
        };
        inline bool IsEmpty() const {
            return this->Top() == nullptr;
        };
        inline void Clear() {
            m_span = TextSpan{};
            m_pClassName = nullptr;
        };
    private:
        TextSpan    m_span;
        const char* m_pClassName;
    };

    inline bool operator<( const HighlightSpan& span1, const HighlightSpan& span2 ) {
        if( span1.Top() < span2.Top() ) return true;
        if( span1.Top() > span2.Top() ) return false;
        if( span2.End() < span1.End() ) return true;
        if( span2.End() > span1.End() ) return false;
        return false;    //should not be reached.
    };

	static void RetrieveCandidate( const char* pTop, const char* pEnd,
                                   const RangeFinderUnit* pUnit, void* pDumData,
                                   void (*callback)( const HighlightSpan&, void* ) );
	static void ClearOverlapSpans( HighlightSpan* pSpanTop, HighlightSpan* pSpanEnd );
    static void WriteHighlightenData( std::ostream& os, const TextSpan& whole,
                                      const HighlightSpan* pSpanTop, const HighlightSpan* pSpanEnd );


    //--------------------------------------------------------------------------
    //
    // implementation of class utility InternalFilter
    //
    //--------------------------------------------------------------------------
    InternalFilterFunc* InternalFilter::FindFilter( const TextSpan& type ) {
        if( type.IsEqual( "raw" ) )     return InternalFilter4Raw;
        if( type.IsEqual( "table" ) )   return InternalFilter4Table;
        if( type.IsEqual( "C" ) ||
            type.IsEqual( "c" ) )       return InternalFilter4C;
        if( type.IsEqual( "C++" ) ||
            type.IsEqual( "c++" ) )     return InternalFilter4Cxx;
        if( type.IsEqual( "diff" ) )    return InternalFilter4Diff;
        if( type.IsEqual( "sh" ) )      return InternalFilter4Shell;
        if( type.IsEqual( "lisp" ) )    return InternalFilter4CommonLISP;
        if( type.IsEqual( "math" ) )    return InternalFilter4MathJax;
        return nullptr;
    }

    void InternalFilter::Execute( std::ostream& os,
                                  const TextSpan& span,
                                  const RangeFinderUnit* pUnit ) {
        std::vector<HighlightSpan> spans;
        // RangeFinderUnit シーケンスを反復
        for( ; pUnit->IsEmpty() == false; ++pUnit ) {
            // 現在の RangeFinderUnit シーケンスの候補を検索してすべて回収
            RetrieveCandidate( span.Top(),
                               span.End(), pUnit, (void*)&spans,
                               []( const HighlightSpan& hlSpan, void* pDumData ) {
                                   auto p = (std::vector<HighlightSpan>*)pDumData;
                                   p->push_back( hlSpan );
                               } );
        }
        // 回収した全 HighlightSpan をソート
        std::sort( spans.begin(), spans.end() );
        // 範囲が重複する HighlightSpan を除去
        if( 0 < spans.size() ) {
            // 削除すべきものをマーク
            ClearOverlapSpans( &(spans[0]), &(spans[0]) + spans.size() );
            // マークされたものを削除
            auto itr = std::remove_if( spans.begin(), spans.end(),
                                       []( const HighlightSpan& span ) -> bool {
                                           return span.IsEmpty();
                                       } );
            spans.erase( itr, spans.end() );
        }
        // 結果を出力
        WriteHighlightenData( os, span, &(spans[0]), &(spans[0]) + spans.size() );
    }

	static void RetrieveCandidate( const char* pTop, const char* pEnd,
                                   const RangeFinderUnit* pUnit, void* pDumData,
                                   void (*callback)( const HighlightSpan&, void* ) ) {
        TextSpan range{ pTop, pEnd };
        TextSpan result{};
        const char* pClassName = pUnit->className;
        while( range.Top() < range.End() ) {
            if( pUnit->finder( range, pUnit->target, result, pClassName ) == false )
                break;
            callback( HighlightSpan{ result, pClassName }, pDumData );
            range = TextSpan{ result.End() + 1, pEnd };
            pClassName = pUnit->className;
        }
    }

	static void ClearOverlapSpans( HighlightSpan* pSpanTop, HighlightSpan* pSpanEnd ) {
        while( pSpanTop + 1 < pSpanEnd ) {
            for( auto pSpan = pSpanTop + 1; pSpan < pSpanEnd; ++pSpan ) {
                if( pSpan->IsOverlap( *pSpanTop ) )
                    pSpan->Clear();
                else {
                    pSpanTop = pSpan;
                    break;
                }
            }
        }
    }
                                   
    static void WriteHighlightenData( std::ostream& os, const TextSpan& whole,
                                      const HighlightSpan* pSpanTop, const HighlightSpan* pSpanEnd ) {
        TextSpan range = whole;
        while( 0 < range.ByteLength() ) {
            // HighlightSpan がもうないなら残りデータを全て出力して終了
            if( pSpanTop == pSpanEnd ) {
                range.WriteSimple( os );
                break;
            }
            // 残りデータの先頭と HighlightSpan シーケンス先頭のデータ位置比較で分岐
            if( range.Top() < pSpanTop->Top() ) {
                // 残りデータが先なら先行部分だけ出力
                TextSpan{ range.Top(), pSpanTop->Top() }.WriteSimple( os );
                range.Top() = pSpanTop->Top();
            } else {
                // 上記以外の場合は HighlightSpan を出力
                os << "<span class='" << pSpanTop->Class() << "'>";    //MEMO : ignore StyleStack.
                pSpanTop->Span().WriteSimple( os );
                os << "</span>";
                range.Top() = pSpanTop->End();
                ++pSpanTop;
            }
        }
    }
    
}
// namespace turnup
