//------------------------------------------------------------------------------
//
// HtmlHeader.hxx
//
//------------------------------------------------------------------------------
#ifndef HTMLHEADER_HXX__
#define HTMLHEADER_HXX__

#include "TextSpan.hxx"

#include <stdint.h>
#include <iosfwd>

namespace turnup {

	class Config;

	//--------------------------------------------------------------------------
	//
	// class HtmlHeader
	//
	//--------------------------------------------------------------------------
	class HtmlHeader {
	public:
		HtmlHeader();
		HtmlHeader( const HtmlHeader& ) = delete;
		~HtmlHeader();
		HtmlHeader& operator=( const HtmlHeader& ) = delete;
	public:
		void SetTitle( const TextSpan& title );
		void SetStyleSheet( const TextSpan& styleSheet );
		std::ostream& WriteTo( std::ostream& os, const Config& cfg ) const;
	private:
		TextSpan m_title;
		TextSpan m_styleSheet;
	};

} // namespace turnup

#endif // HTMLHEADER_HXX__
