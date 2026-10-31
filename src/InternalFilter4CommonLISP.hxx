//------------------------------------------------------------------------------
//
// InternalFilter4CommonLISP.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4COMMONLISP_HXX_
#define SRC_INTERNALFILTER4COMMONLISP_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4CommonLISP(std::ostream& os, DocumentInfo& docInfo,
                               const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4COMMONLISP_HXX_
