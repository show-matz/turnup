//------------------------------------------------------------------------------
//
// InternalFilter4Table.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4TABLE_HXX__
#define INTERNALFILTER4TABLE_HXX__

#include "InternalFilter.hxx"

namespace turnup {

	bool InternalFilter4Table( std::ostream& os, DocumentInfo& docInfo,
							   const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4TABLE_HXX__
