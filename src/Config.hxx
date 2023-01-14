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
		bool	bEmbedStyleSheet;
		bool	bWriteComment;
		bool	bNumberingHeader;
		bool	bUseMathJax;
		uint8_t	minNumberingLv;
		uint8_t	maxNumberingLv;
		uint8_t	entityNumberingDepth;
	};

} // namespace turnup

#endif // CONFIG_HXX__
