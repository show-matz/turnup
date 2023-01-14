//------------------------------------------------------------------------------
//
// InternalFilter4MathJax.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER4MATHJAX_HXX__
#define INTERNALFILTER4MATHJAX_HXX__

#include "InternalFilter.hxx"

namespace turnup {

	bool InternalFilter4MathJax( std::ostream& os, DocumentInfo& docInfo,
								 const TextSpan* pTop, const TextSpan* pEnd );

} // namespace turnup

#endif // INTERNALFILTER4MATHJAX_HXX__
