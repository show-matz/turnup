//------------------------------------------------------------------------------
//
// InternalFilter4Default.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4DEFAULT_HXX_
#define SRC_INTERNALFILTER4DEFAULT_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4Default(std::ostream& os, DocumentInfo& docInfo,
                            const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4DEFAULT_HXX_
