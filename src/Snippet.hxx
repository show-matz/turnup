//------------------------------------------------------------------------------
//
// Snippet.hxx
//
//------------------------------------------------------------------------------
#ifndef SNIPPET_HXX__
#define SNIPPET_HXX__

#include "TextSpan.hxx"
#include <vector>

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class Snippet
    //
    //--------------------------------------------------------------------------
    class Snippet {
    public:
        static void Expand( std::vector<TextSpan>& lines );
    };


} // namespace turnup

#endif // SNIPPET_HXX__
