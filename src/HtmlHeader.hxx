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
		std::ostream& WriteTo( std::ostream& os ) const;
	private:
		const char* m_pTitle;
		const char* m_pStyleSheet;
	};

	std::ostream& operator<<( std::ostream& os, const HtmlHeader& header );

} // namespace turnup

#endif // HTMLHEADER_HXX__
