//------------------------------------------------------------------------------
//
// InternalFilter4Default.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4DEFAULT_HXX__
#define INTERNALFILTER4DEFAULT_HXX__

#include "InternalFilter.hxx"

namespace turnup {

    bool InternalFilter4Default( std::ostream& os, DocumentInfo& docInfo,
                                 const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4DEFAULT_HXX__
