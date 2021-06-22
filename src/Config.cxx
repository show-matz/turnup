//------------------------------------------------------------------------------
//
// Config.cxx
//
//------------------------------------------------------------------------------
#include "Config.hxx"

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class Config
	//
	//--------------------------------------------------------------------------
	Config::Config() : bTermLinkInHeader( false ),
					   bNumberingHeader( false ),
					   minNumberingLv( 1 ),
					   maxNumberingLv( 6 ) {
	}

	Config::~Config() {
	}

} // namespace turnup

