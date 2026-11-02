//------------------------------------------------------------------------------
//
// InternalFilter4Attach.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4ATTACH_HXX_
#define SRC_INTERNALFILTER4ATTACH_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4Attach(std::ostream& os, DocumentInfo& docInfo,
                           const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4ATTACH_HXX_
