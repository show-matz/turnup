//------------------------------------------------------------------------------
//
// Snippet.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_SNIPPET_HXX_
#define SRC_SNIPPET_HXX_

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
    static void Expand(std::vector<TextSpan>& lines);
};

}  // namespace turnup

#endif  // SRC_SNIPPET_HXX_
