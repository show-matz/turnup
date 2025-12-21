//------------------------------------------------------------------------------
//
// Operator4RawHTML.cxx
//
//------------------------------------------------------------------------------
#include "Operator4RawHTML.hxx"

#include "DocumentInfo.hxx"
#include "TextSpan.hxx"
#include "InternalFilter4Default.hxx"

#include <iostream>

namespace turnup {

    const TextSpan* Operator4RawHTML( const TextSpan* pTop,
                                      const TextSpan* pEnd, DocumentInfo& docInfo ) {

        if( pTop->TrimTail().IsEqual( "<raw_html>" ) == false )
            return pTop;

        const TextSpan* pLine = ++pTop;
        for( ; pLine < pEnd; ++pLine ) {
            if( pLine->TrimTail().IsEqual( "</raw_html>" ) )
                break;
        }
        if( pLine == pEnd ) {
            std::cerr << "ERROR : </raw_html> is missing." << std::endl;
            return pEnd;
        }

        // safe mode か否かで分岐
        if( docInfo.IsSafeMode() == false ) {
            std::cout << "<!-- start raw html -->" << std::endl;
            for( ; pTop < pLine; ++pTop ) {
                // TextSpan::WriteTo() は文字エスケープなどをするのでここでは避ける
                std::cout.write( pTop->Top(), pTop->ByteLength() );
                std::cout << std::endl;
            }
            std::cout << "<!-- end raw html -->" << std::endl;
    
            // この機能は obsolete となったので警告を出しておく
            std::cerr << "WARNING : <raw_html> is obsolete. use internal filter 'raw'." << std::endl;

        } else {
            InternalFilter4Default( std::cout, docInfo, pTop, pLine );
            // safe-mode で raw HTML が使用されたので警告を出しておく
            std::cerr << "WARNING : <raw_html> is used in safe-mode." << std::endl;
        }

        return pLine + 1;
    }


} // namespace turnup

