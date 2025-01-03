//------------------------------------------------------------------------------
//
// StringReplacer.cxx
//
//------------------------------------------------------------------------------
#include "StringReplacer.hxx"

#include "TextMaker.hxx"

#include <algorithm>

namespace turnup {

    static const char* FindPlaceHolder( const char* pTop, 
                                        const char* pEnd, uint32_t& index );

    //--------------------------------------------------------------------------
    //
    // implementation of class StringReplacer
    //
    //--------------------------------------------------------------------------
    StringReplacer::StringReplacer( const TextSpan* pTop,
                                    const TextSpan* pEnd ) : m_pTop( pTop ),
                                                             m_pEnd( pEnd ) {
    }

    StringReplacer::~StringReplacer() {
    }

    TextSpan StringReplacer::operator()( const TextSpan& line ) {
        const char* pTop = line.Top();
        const char* pEnd = line.End();
        uint32_t    idx  = 0;
        //最初のプレースホルダを検索
        auto p = FindPlaceHolder( pTop, pEnd, idx );
        if( p == pEnd )
            // なければ line をそのまま返す
            return line;
        else {
            // みつかった場合、TextMaker を使った置換を開始
            TextMaker tm{};
            do {
                tm  << TextSpan{ pTop, p }
                    << ((m_pEnd - m_pTop) <= idx ? TextSpan{} : m_pTop[idx]);
                pTop = p + 2;
                p = FindPlaceHolder( pTop, pEnd, idx );
                if( p == pEnd ) {
                    tm << TextSpan{ pTop, pEnd };
                    break;
                }
            } while( true );
            return tm.GetSpan();
        }
    }

    //--------------------------------------------------------------------------
    //
    // local function
    //
    //--------------------------------------------------------------------------
    static const char* FindPlaceHolder( const char* pTop,
                                        const char* pEnd, uint32_t& index ) {
        while( pTop < pEnd ) {
            const char* p = std::find( pTop, pEnd, '%' );
            if( p < (pEnd-1) && '0' < p[1] && p[1] <= '9' ) {
                index = p[1] - '1';
                return p;
            }
            pTop = p + 1;
        }
        return pEnd;
    }


} // namespace turnup

