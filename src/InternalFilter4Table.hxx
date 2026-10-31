//------------------------------------------------------------------------------
//
// InternalFilter4Table.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INTERNALFILTER4TABLE_HXX_
#define SRC_INTERNALFILTER4TABLE_HXX_

#include "InternalFilter.hxx"

namespace turnup {

bool InternalFilter4Table(std::ostream& os, DocumentInfo& docInfo,
                          const TextSpan* pTop, const TextSpan* pEnd);

}  // namespace turnup

#endif  // SRC_INTERNALFILTER4TABLE_HXX_
