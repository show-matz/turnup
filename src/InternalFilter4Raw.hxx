//------------------------------------------------------------------------------
//
// InternalFilter4Raw.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4RAW_HXX_
#define SRC_INTERNALFILTER4RAW_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4Raw(std::ostream& os, DocumentInfo& docInfo,
                        const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4RAW_HXX_
