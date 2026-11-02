//------------------------------------------------------------------------------
//
// InternalFilter4Shell.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4SHELL_HXX_
#define SRC_INTERNALFILTER4SHELL_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4Shell(std::ostream& os, DocumentInfo& docInfo,
                          const TextSpan* pTop, const TextSpan* pEnd);


}  // namespace turnup

#endif  // SRC_INTERNALFILTER4SHELL_HXX_
