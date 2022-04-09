//------------------------------------------------------------------------------
//
// InternalFilter4Raw.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4RAW_HXX__
#define INTERNALFILTER4RAW_HXX__

#include "InternalFilter.hxx"

namespace turnup {

	bool InternalFilter4Raw( std::ostream& os, DocumentInfo& docInfo,
							 const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4RAW_HXX__
