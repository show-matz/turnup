//------------------------------------------------------------------------------
//
// Operator4Anchor.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Anchor.hxx"

#include "DocumentInfo.hxx"
#include "TextSpan.hxx"
#include "ToC.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

    const TextSpan* Operator4Anchor( const TextSpan* pTop,
                                     const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;

        TextSpan line = pTop->Trim();
        TextSpan title;
        if( line.IsMatch( "<!-- anchor:", title, " -->" ) == false )
            return pTop;

        title = title.Trim();
        auto& toc    = docInfo.Get<ToC>();
        bool  pDuplicated = false;
        const char* pTag = toc.GetAnchorTag( ToC::EntryT::ANCHOR,
                                             pDuplicated, title.Top(), title.End() );
        //Duplicated なアンカーでも、ここではリンク先を生成するだけなのでエラーにはしない
        if( pTag )
            std::cout << "<a name='" << pTag << "'></a>" << std::endl;
        else {
            //ToDo : error message...
        }
        return pTop + 1;
    }

} // namespace turnup

