//------------------------------------------------------------------------------
//
// InternalFilter4Shell.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4SHELL_HXX__
#define INTERNALFILTER4SHELL_HXX__

#include "InternalFilter.hxx"

namespace turnup {

    bool InternalFilter4Shell( std::ostream& os, DocumentInfo& docInfo,
                               const TextSpan* pTop, const TextSpan* pEnd );


} // namespace turnup

#endif // INTERNALFILTER4SHELL_HXX__
