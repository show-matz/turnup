//------------------------------------------------------------------------------
//
// Operator4Paragraph.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Paragraph.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "StyleStack.hxx"

#include <iostream>

namespace turnup {

    const TextSpan* Operator4Paragraph( const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo ) {
        //MEMO : 基本的にこのオペレータは last resort なので、空行でない限りはすべて処理する。
        auto& styles = docInfo.Get<StyleStack>();
        styles.WriteOpenTag( std::cout, "p" );
        for( ; pTop < pEnd; ++pTop ) {
            if( pTop->IsEmpty() )
                break;
            if( pTop->TrimHead().BeginWith( "<!--" ) )
                break;
            pTop->WriteTo( std::cout, docInfo );
        }
        std::cout << "</p>" << std::endl;
        return pTop;
    }


} // namespace turnup

