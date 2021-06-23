//------------------------------------------------------------------------------
//
// HtmlHeader.hxx
//
//------------------------------------------------------------------------------
#ifndef HTMLHEADER_HXX__
#define HTMLHEADER_HXX__

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
		void SetTitle( const char* pTitle );
		void SetStyleSheet( const char* pStyleSheet );
		std::ostream& WriteTo( std::ostream& os, const Config& cfg ) const;
	private:
		const char* m_pTitle;
		const char* m_pStyleSheet;
	};

} // namespace turnup

#endif // HTMLHEADER_HXX__
