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
	typedef bool RangeFinder( const TextSpan& span, const char* pTarget, 
							  TextSpan& result, const char*& className );

	struct RangeFinderUnit {
		RangeFinder*	finder;
		const char*		target;
		const char*		className;
	};

	//--------------------------------------------------------------------------
	//
	// class utility InternalFilter
	//
	//--------------------------------------------------------------------------
	class InternalFilter {
	public:
		static InternalFilterFunc* FindFilter( const TextSpan& type );
		static void ExecRecursive( std::ostream& os,
								   const TextSpan& span,
								   const RangeFinderUnit* pUnit );

	};

} // namespace turnup

#endif // INTERNALFILTER_HXX__
