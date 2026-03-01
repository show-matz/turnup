//------------------------------------------------------------------------------
//
// InternalFilter4Attach.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4ATTACH_HXX__
#define INTERNALFILTER4ATTACH_HXX__

#include "InternalFilter.hxx"

namespace turnup {

    bool InternalFilter4Attach( std::ostream& os, DocumentInfo& docInfo,
                                const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4ATTACH_HXX__
