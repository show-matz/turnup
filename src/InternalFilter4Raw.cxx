//------------------------------------------------------------------------------
//
// InternalFilter4Raw.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Raw.hxx"

#include "DocumentInfo.hxx"
#include "TextSpan.hxx"
#include "InternalFilter4Default.hxx"

#include <iostream>

namespace turnup {

    bool InternalFilter4Raw( std::ostream& os, DocumentInfo& docInfo,
                             const TextSpan* pTop, const TextSpan* pEnd ) {
        if( docInfo.IsSafeMode() == false ) {
            for( ; pTop < pEnd; ++pTop )
                os << *pTop << std::endl;
        } else {
            InternalFilter4Default( os, docInfo, pTop, pEnd );
            // safe-mode で raw HTML が使用されたので警告を出しておく
            std::cerr << "WARNING : raw filter is used in safe-mode." << std::endl;
        }
        return true;
    }

} // namespace turnup
