//------------------------------------------------------------------------------
//
// Config.cxx
//
//------------------------------------------------------------------------------
#include "Config.hxx"

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class Config
    //
    //--------------------------------------------------------------------------
    Config::Config() : bTermLinkInHeader( false ),
                       bEmbedStyleSheet( false ),
                       bWriteComment( false ),
                       bNumberingHeader( false ),
                       bUseMathJax( false ),
                       bEntityNumbering( true ),
                       minNumberingLv( 1 ),
                       maxNumberingLv( 6 ),
                       entityNumberingDepth( 0 ) {
    }

    Config::~Config() {
    }

} // namespace turnup

