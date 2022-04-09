//------------------------------------------------------------------------------
//
// InternalFilter4CandCxx.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4CANDCXX_HXX__
#define INTERNALFILTER4CANDCXX_HXX__

#include "InternalFilter.hxx"

namespace turnup {

	bool InternalFilter4C( std::ostream& os, DocumentInfo& docInfo,
						   const TextSpan* pTop, const TextSpan* pEnd );
	bool InternalFilter4Cxx( std::ostream& os, DocumentInfo& docInfo,
							 const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4CANDCXX_HXX__
