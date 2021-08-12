//------------------------------------------------------------------------------
//
// InternalFilter.hxx
//
//------------------------------------------------------------------------------
#ifndef INTERNALFILTER_HXX__
#define INTERNALFILTER_HXX__

#include <iosfwd>

namespace turnup {

	class TextSpan;

	typedef bool InternalFilterFunc( std::ostream& os,
									 const TextSpan* pTop, const TextSpan* pEnd );

	//--------------------------------------------------------------------------
	//
	// class utility InternalFilter
	//
	//--------------------------------------------------------------------------
	class InternalFilter {
	public:
		static InternalFilterFunc* FindFilter( const TextSpan& type );
	};

} // namespace turnup

#endif // INTERNALFILTER_HXX__
