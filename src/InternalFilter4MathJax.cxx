//------------------------------------------------------------------------------
//
// InternalFilter4MathJax.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4MathJax.hxx"

#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

    bool InternalFilter4MathJax( std::ostream& os, DocumentInfo& docInfo,
                                 const TextSpan* pTop, const TextSpan* pEnd ) {
        (void)docInfo;
        os << "\\[" << std::endl;
        for( ; pTop < pEnd; ++pTop )
            os << *pTop << std::endl;
        os << "\\]" << std::endl;
        return true;
    }

} // namespace turnup
