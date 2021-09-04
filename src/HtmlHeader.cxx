//------------------------------------------------------------------------------
//
// HtmlHeader.cxx
//
//------------------------------------------------------------------------------
#include "HtmlHeader.hxx"

#include "Config.hxx"
#include "InputData.hxx"

#include <iostream>
#include <string.h>

namespace turnup {

	static void EmbedStyleSheet( std::ostream& os, const char* pStyleSheet );

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

	std::ostream& HtmlHeader::WriteTo( std::ostream& os, const Config& cfg ) const {
		std::cout << "<header>" << std::endl;
		std::cout << "	<meta http-equiv=\"Content-Type\""
							" content=\"text/html; charset=utf-8\">" << std::endl;
		std::cout << "	<meta http-equiv=\"Content-Style-Type\""
							" content=\"text/css\">" << std::endl;
		if( !!m_pTitle )
			std::cout << "	<title>" << m_pTitle << "</title>" << std::endl;
		if( !!m_pStyleSheet ) {
			if( cfg.bEmbedStyleSheet )
				EmbedStyleSheet( os, m_pStyleSheet );
			else
				std::cout << "	<link rel=stylesheet href=\"" << m_pStyleSheet << "\">" << std::endl;
		}
		std::cout << "</header>" << std::endl;
		return os;
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static void EmbedStyleSheet( std::ostream& os, const char* pStyleSheet ) {

		// file missing などの理由でロード失敗の場合は InputData::Create() 内部で errmsg 出してる
		InputData* pCssFile = InputData::Create( pStyleSheet );
		if( !pCssFile )
			return;

		os << "  <style>" << std::endl;
		os << "  <!--" << std::endl;
		
		auto itr1 = pCssFile->Begin();
		auto itr2 = pCssFile->End();
		for( ; itr1 < itr2; ++itr1 ) {
			os.write( itr1->Top(), itr1->ByteLength() );
			os << std::endl;
		}
		os << "  -->" << std::endl;
		os << "  </style>" << std::endl;
		InputData::Release( pCssFile );
	}

} // namespace turnup
