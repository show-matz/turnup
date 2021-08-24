//------------------------------------------------------------------------------
//
// Operator4Table.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Table.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "StylePalette.hxx"
#include "TextSpan.hxx"

#include <stdint.h>
#include <iostream>
#include <vector>
#include <algorithm>

namespace turnup {

	static void WriteWithPalette( std::ostream& os, const char* tag,
								  const TextSpan* pPalette, const char* pStyle = nullptr );

	//--------------------------------------------------------------------------
	//
	// enum Align & class Alignments
	//
	//--------------------------------------------------------------------------
	enum Align {
		ALIGN_LEFT   = 0,
		ALIGN_CENTER = 1,
		ALIGN_RIGHT  = 2
	};
	class Alignments {
	public:
		Alignments();
		~Alignments();
	public:
		bool Load( const TextSpan& line );
		Align Get( uint32_t i, bool& bNoWrap ) const;
	private:
		void Set( uint32_t i, Align align, bool bNoWrap );
		void Clear();
	private:
		std::vector<uint32_t>* m_pColumns;
	};

	Alignments s_aligns;

	//--------------------------------------------------------------------------
	//
	// exported functions
	//
	//--------------------------------------------------------------------------
	const TextSpan* Operator4Table( const TextSpan* pTop,
									const TextSpan* pEnd, DocumentInfo& docInfo ) {
		static const char* const s_tags[]   = { "th", "td" };
		static const char* const s_styles[] = {
			" nowrap align='left'",
			" nowrap align='center'",
			" nowrap align='right'"
		};
		TextSpan line = pTop->TrimHead();
		if( line[0] != '|' )
			return nullptr;

		auto& styles = docInfo.Get<StyleStack>();
		auto& palette = docInfo.Get<StylePalette>();
		styles.WriteOpenTag( std::cout, "table", " align='center'" ) << std::endl;
		for( uint32_t row = 0; pTop < pEnd; ++pTop, ++row ) {

			line = pTop->Trim();

			uint32_t idx = 10;	// palette index : 10 means 'not specified'.
			TextSpan tmp;
			TextSpan rest;

			// 行頭が [N| で始まる場合に tr にスタイル付けをする
			if( line.IsMatch( "[", tmp, "|", rest, "" ) &&
					tmp.ByteLength() == 1 && '0' <= tmp[0] && tmp[0] <= '9' ) {
				idx  = (tmp[0] - '0');
				line = TextSpan{ rest.Top() - 1, rest.End() };
			}
			if( line[0] != '|' )
				break;

			if( row == 1 && s_aligns.Load( line ) )
				continue;

			if( idx == 10 )
				styles.WriteOpenTag( std::cout, "tr" );
			else
				WriteWithPalette( std::cout, "tr", palette.GetStyle( idx ) );

			auto p1 = line.Top();
			auto p2 = line.End();
			for( uint32_t col = 0;  p1 < p2; ++col ) {
				if( *p1 == '|' && p1+1 == p2 )
					break;
				idx = 10;
				if( p1[2] == ']' && '0' <= p1[1] && p1[1] <= '9' ) {
					idx = p1[1] - '0';
					p1 += 2;
				}
				auto pDelim = std::find( p1 + 1, p2, '|' );
				auto item   = TextSpan{ p1 + 1, pDelim }.Trim();
				bool bNoWrap = false; 
				auto align  = s_aligns.Get( col, bNoWrap );
				const char* pDefaultStyle = "";
				if( 0 < row ) {
					pDefaultStyle =  s_styles[align];
					if( !bNoWrap )
						pDefaultStyle += 7;

				}
				if( idx == 10 )
					styles.WriteOpenTag( std::cout, s_tags[!!row], pDefaultStyle );
				else
					WriteWithPalette( std::cout, s_tags[!!row],
									  palette.GetStyle( idx ), pDefaultStyle );
				item.WriteTo( std::cout, docInfo );
				std::cout << "</" << s_tags[!!row] << '>';
				p1 = pDelim;
			}
			std::cout << "</tr>" << std::endl;
		}
		std::cout << "</table>" << std::endl;
		return pTop;
	}



	//--------------------------------------------------------------------------
	//
	// implementation of class Alignments
	//
	//--------------------------------------------------------------------------
	Alignments::Alignments() : m_pColumns( nullptr ) {
	}
	Alignments::~Alignments() {
		if( m_pColumns ) {
			m_pColumns->clear();
			delete m_pColumns;
		}
	}
	bool Alignments::Load( const TextSpan& line ) {
		auto chk1 = []( char c ) {
			return c == '-' || c == '=' || c == '|' || c == ':' || c == ' ';
		};
		if( std::all_of( line.Top(), line.End(), chk1 ) == false )
			return false;
		this->Clear();

		auto pred1 = []( char c ) { return c == '-' || c == '='; };
		auto pred2 = []( char c ) { return c == '='; };
		auto p1 = line.Top();
		auto p2 = line.End();
		for( uint32_t col = 0;  p1 < p2; ++col ) {
			if( *p1 == '|' && p1+1 == p2 )
				break;
			auto pDelim = std::find( p1 + 1, p2, '|' );
			auto item   = TextSpan{ p1 + 1, pDelim }.Trim();
			auto p3     = item.Top();
			auto p4     = item.End();
			if( p3[0] == ':' && p4[-1] == ':' && std::all_of( p3+1, p4-1, pred1 ) ) {
				this->Set( col, ALIGN_CENTER,
						   std::all_of( p3+1, p4-1, pred2 ) );
			} else if( p4[-1] == ':' && std::all_of( p3, p4-1, pred1 ) ) {
				this->Set( col, ALIGN_RIGHT,
						   std::all_of( p3+1, p4-1, pred2 ) );
			} else if( p3[0] == ':' && std::all_of( p3+1, p4, pred1 ) ) {
				this->Set( col, ALIGN_LEFT,
						   std::all_of( p3+1, p4-1, pred2 ) );
			} else
				this->Set( col, ALIGN_LEFT, false );
			p1 = pDelim;
		}
		return true;
	}
	Align Alignments::Get( uint32_t i, bool& bNoWrap ) const {
		if( !m_pColumns || m_pColumns->size() <= i ) {
			bNoWrap = false;
			return ALIGN_LEFT;
		}
		uint32_t v = (*m_pColumns)[i];
		bNoWrap = !!(v & 0x01);
		return static_cast<Align>( v >> 1 );
	}
	void Alignments::Set( uint32_t i, Align align, bool bNoWrap ) {
		if( !m_pColumns )
			m_pColumns = new std::vector<uint32_t>{};
		if( m_pColumns->size() <= i )
			m_pColumns->resize( i+1, ALIGN_LEFT );
		(*m_pColumns)[i] = (static_cast<uint32_t>( align ) << 1) | bNoWrap;
	}
	void Alignments::Clear() {
		if( !!m_pColumns )
			m_pColumns->clear();
	}


	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static void WriteWithPalette( std::ostream& os, const char* tag,
								  const TextSpan* pPalette, const char* pStyle ) {
		os << '<' << tag;
		if( !!pPalette ) {
			os << ' ';
			os.write( pPalette->Top(), pPalette->ByteLength() );
		}
		if( !!pStyle )
			os << pStyle;
		os << ">";
	}

} // namespace turnup

