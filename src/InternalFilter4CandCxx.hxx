//------------------------------------------------------------------------------
//
// InternalFilter4CandCxx.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4CANDCXX_HXX_
#define SRC_INTERNALFILTER4CANDCXX_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4C(std::ostream& os, DocumentInfo& docInfo,
                      const TextSpan* pTop, const TextSpan* pEnd);
bool InternalFilter4Cxx(std::ostream& os, DocumentInfo& docInfo,
                        const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4CANDCXX_HXX_
