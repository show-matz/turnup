//------------------------------------------------------------------------------
//
// InternalFilter4CommonLISP.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4COMMONLISP_HXX__
#define INTERNALFILTER4COMMONLISP_HXX__

#include "InternalFilter.hxx"

namespace turnup {

	bool InternalFilter4CommonLISP( std::ostream& os, DocumentInfo& docInfo,
									const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4COMMONLISP_HXX__
