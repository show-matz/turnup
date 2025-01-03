//------------------------------------------------------------------------------
//
// Operator4Error.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Error.hxx"

#include "TextSpan.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

    const TextSpan* Operator4Error( const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;
        (void)docInfo;

        TextSpan line = pTop->Trim();
        TextSpan msg;
        if( line.IsMatch( "<!-- error:", msg, " -->" ) ) {
            std::cout << "<p><span style='background:pink; color:red;'>"
                      << "ERROR : " << msg
                      << "</span></p>" << std::endl;
            std::cerr << "ERROR : " << msg << std::endl;
            return nullptr;    // nullptr means 'abort by error'.
        }
        if( line.IsMatch( "<!-- warning:", msg, " -->" ) ) {
            std::cout << "<p><span style='background:pink; color:red;'>"
                      << "WARNING : " << msg
                      << "</span></p>" << std::endl;
            std::cerr << "WARNING : " << msg << std::endl;
            return pTop + 1;
        }
        return pTop;
    }

} // Namespace turnup

