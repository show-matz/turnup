//------------------------------------------------------------------------------
//
// InternalFilter.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter.hxx"

#include "InternalFilter4Diff.hxx"

#include "TextSpan.hxx"

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class utility InternalFilter
	//
	//--------------------------------------------------------------------------
	InternalFilterFunc* InternalFilter::FindFilter( const TextSpan& type ) {
		if( type.IsEqual( "diff" ) )	return InternalFilter4Diff;
		return nullptr;
	}

} // namespace turnup
