//------------------------------------------------------------------------------
//
// HtmlHeader.cxx
//
//------------------------------------------------------------------------------
#include "HtmlHeader.hxx"

#include "Config.hxx"
#include "InputFile.hxx"
#include "TextSpan.hxx"

#include <iostream>
#include <string.h>

namespace turnup {

	static void EmbedStyleSheet( std::ostream& os, const TextSpan& styleSheet );

	//--------------------------------------------------------------------------
	//
	// implementation of class HtmlHeader
	//
	//--------------------------------------------------------------------------
	HtmlHeader::HtmlHeader() : m_title(),
							   m_styleSheet() {
	}

	HtmlHeader::~HtmlHeader() {
	}

	void HtmlHeader::SetTitle( const TextSpan& title ) {
		m_title = title;
	}
	
	void HtmlHeader::SetStyleSheet( const TextSpan& styleSheet ) {
		m_styleSheet = styleSheet;
	}

	std::ostream& HtmlHeader::WriteTo( std::ostream& os, const Config& cfg ) const {
		os << "<head>" << std::endl;
		os << "	<meta http-equiv=\"Content-Type\""
							" content=\"text/html; charset=utf-8\">" << std::endl;
		os << "	<meta http-equiv=\"Content-Style-Type\""
							" content=\"text/css\">" << std::endl;
		if( m_title.IsEmpty() == false ) {
			os << "	<title>";
			os.write( m_title.Top(),
					  m_title.ByteLength() ) << "</title>" << std::endl;
		}
		if( m_styleSheet.IsEmpty() == false ) {
			if( cfg.bEmbedStyleSheet )
				EmbedStyleSheet( os, m_styleSheet );
			else {
				os << "	<link rel=stylesheet href=\"";
				os.write( m_styleSheet.Top(),
						  m_styleSheet.ByteLength() ) << "\">" << std::endl;
			}
		}
		if( cfg.bUseMathJax ) {
			os << "	<script type=\"text/javascript\" "
				  "id=\"MathJax-script\" "
				  "async "
				  "src=\"https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-chtml.js\">"
				  "</script>" << std::endl;
		}
		os << "</head>" << std::endl;
		return os;
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static void EmbedStyleSheet( std::ostream& os, const TextSpan& styleSheet ) {

		InputFile* pCssFile = InputFile::LoadInputFile( styleSheet );
		if( !pCssFile ) {
			std::cerr << "ERROR : Failure loading file '";
				os.write( styleSheet.Top(),
						  styleSheet.ByteLength() ) << "'." << std::endl;
			return;
		}

		os << "  <style>" << std::endl;
		os << "  <!--" << std::endl;
		
		auto itr1 = pCssFile->LineTop();
		auto itr2 = pCssFile->LineEnd();
		for( ; itr1 < itr2; ++itr1 ) {
			os.write( itr1->Top(), itr1->ByteLength() );
			os << std::endl;
		}
		os << "  -->" << std::endl;
		os << "  </style>" << std::endl;
		InputFile::ReleaseInputFile( pCssFile );
	}

} // namespace turnup
