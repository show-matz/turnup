//------------------------------------------------------------------------------
//
// Operator4HorizontalLine.cxx
//
//------------------------------------------------------------------------------
#include "Operator4HorizontalLine.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "Utilities.hxx"

#include <iostream>
#include <algorithm>

namespace turnup {

    const TextSpan* Operator4HorizontalLine( const TextSpan* pTop,
                                             const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;

        char cands[] = { '-', '=', '_', '*' };
        for( uint32_t i = 0; i < count_of(cands); ++i ) {
            uint32_t cnt = pTop->CountTopOf( cands[i] );
            if( cnt < 3 )
                continue;
            if( cnt < pTop->ByteLength() )
                continue;
            auto& styles = docInfo.Get<StyleStack>();
            styles.WriteOpenTag( std::cout, "hr" ) << std::endl;
            return pTop + 1;
        }
        return pTop;
    }


} // namespace turnup

