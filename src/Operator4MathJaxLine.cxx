//------------------------------------------------------------------------------
//
// Operator4MathJaxLine.cxx
//
//------------------------------------------------------------------------------
#include "Operator4MathJaxLine.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

    const TextSpan* Operator4MathJaxLine( const TextSpan* pTop,
                                          const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;
        (void)docInfo;
        TextSpan line = pTop->Trim();
        TextSpan contents;
        if( line.IsMatch( "$$", contents, "$$" ) == false )
            return pTop;

        std::cout << "\\[" << std::endl;
        std::cout << contents << std::endl;
        std::cout << "\\]" << std::endl;
        return pTop + 1;
    }


} // namespace turnup

