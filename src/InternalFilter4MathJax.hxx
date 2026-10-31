//------------------------------------------------------------------------------
//
// InternalFilter4MathJax.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4MATHJAX_HXX_
#define SRC_INTERNALFILTER4MATHJAX_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4MathJax(std::ostream& os, DocumentInfo& docInfo,
                            const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4MATHJAX_HXX_
