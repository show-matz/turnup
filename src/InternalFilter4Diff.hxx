//------------------------------------------------------------------------------
//
// InternalFilter4Diff.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4DIFF_HXX__
#define INTERNALFILTER4DIFF_HXX__

#include "InternalFilter.hxx"

namespace turnup {

	bool InternalFilter4Diff( std::ostream& os, DocumentInfo& docInfo,
							  const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4DIFF_HXX__
