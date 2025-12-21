//------------------------------------------------------------------------------
//
// InternalFilter4Default.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Default.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"

#include <iostream>

namespace turnup {

    bool InternalFilter4Default( std::ostream& os, DocumentInfo& docInfo,
                                 const TextSpan* pTop, const TextSpan* pEnd ) {
        auto& styles = docInfo.Get<StyleStack>();
        styles.WriteOpenTag( std::cout, "pre" ) << std::endl;
        for( ; pTop < pEnd; ++pTop ) {
            pTop->WriteSimple( os ) << std::endl;
        }
        os << "</pre>" << std::endl;
        return true;
    }

} // namespace turnup
