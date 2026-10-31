//------------------------------------------------------------------------------
//
// InternalFilter4Diff.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4DIFF_HXX_
#define SRC_INTERNALFILTER4DIFF_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4Diff(std::ostream& os, DocumentInfo& docInfo,
                         const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4DIFF_HXX_
