//------------------------------------------------------------------------------
//
// HtmlHeader.cxx
//
//------------------------------------------------------------------------------
#include "HtmlHeader.hxx"

#include <iostream>
#include <string.h>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class HtmlHeader
	//
	//--------------------------------------------------------------------------
	HtmlHeader::HtmlHeader() : m_pTitle( nullptr ),
							   m_pStyleSheet( nullptr ) {
	}

	HtmlHeader::~HtmlHeader() {
	}

	void HtmlHeader::SetTitle( const char* pTitle ) {
		m_pTitle = pTitle;
	}
	
	void HtmlHeader::SetStyleSheet( const char* pStyleSheet ) {
		m_pStyleSheet = pStyleSheet;
	}

	std::ostream& HtmlHeader::WriteTo( std::ostream& os ) const {
		std::cout << "<header>" << std::endl;
		if( !!m_pStyleSheet )
			std::cout << "	<link rel=stylesheet href=\"" << m_pStyleSheet << "\">" << std::endl;
		std::cout << "	<meta http-equiv=\"Content-Type\""
							" content=\"text/html; charset=utf-8\">" << std::endl;
		std::cout << "	<meta http-equiv=\"Content-Style-Type\""
							" content=\"text/css\">" << std::endl;
		if( !!m_pTitle )
			std::cout << "	<title>" << m_pTitle << "</title>" << std::endl;
		std::cout << "</header>";
		return os;
	}

	std::ostream& operator<<( std::ostream& os, const HtmlHeader& header ) {
		return header.WriteTo( os );
	}

} // namespace turnup
