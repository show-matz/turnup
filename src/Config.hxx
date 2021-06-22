//------------------------------------------------------------------------------
//
// Config.hxx
//
//------------------------------------------------------------------------------
#ifndef CONFIG_HXX__
#define CONFIG_HXX__

#include <stdint.h>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class Config
	//
	//--------------------------------------------------------------------------
	class Config {
	public:
		Config();
		~Config();
	public:
		bool	bTermLinkInHeader;
		bool	bNumberingHeader;
		uint8_t	minNumberingLv;
		uint8_t	maxNumberingLv;
	};

} // namespace turnup

#endif // CONFIG_HXX__
