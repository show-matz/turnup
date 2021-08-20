//------------------------------------------------------------------------------
//
// Operator4Table.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Table.hxx"

#include "TextSpan.hxx"

#include <stdint.h>
#include <iostream>
#include <vector>
#include <algorithm>

namespace turnup {

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
		static const char* const s_labels[] = { "left", "center", "right" };
		TextSpan line = pTop->TrimHead();
		if( line[0] != '|' )
			return nullptr;

		std::cout << "<table align='center'>" << std::endl;
		for( uint32_t row = 0; pTop < pEnd; ++pTop, ++row ) {

			line = pTop->Trim();
			if( line[0] != '|' )
				break;

			if( row == 1 && s_aligns.Load( line ) )
				continue;

			std::cout << "<tr>";
			auto p1 = line.Top();
			auto p2 = line.End();
			for( uint32_t col = 0;  p1 < p2; ++col ) {
				if( *p1 == '|' && p1+1 == p2 )
					break;
				auto pDelim = std::find( p1 + 1, p2, '|' );
				auto item   = TextSpan{ p1 + 1, pDelim }.Trim();
				bool bNoWrap = false; 
				auto align  = s_aligns.Get( col, bNoWrap );
				std::cout << '<' << s_tags[!!row];
				if( 0 < row ) {
					if( bNoWrap )
						std::cout << " nowrap";
					std::cout << " align='" << s_labels[align] << "'";
				}
				std::cout << '>';
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


} // namespace turnup

