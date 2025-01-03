//------------------------------------------------------------------------------
//
// Operator4EmptyLine.cxx
//
//------------------------------------------------------------------------------
#include "Operator4EmptyLine.hxx"

#include "TextSpan.hxx"

namespace turnup {

    const TextSpan* Operator4EmptyLine( const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)docInfo;
        for( ; pTop < pEnd; ++pTop ) {
            if( pTop->IsEmpty() == false )
                break;
        }
        return pTop;
    }


} // namespace turnup

