//------------------------------------------------------------------------------
//
// Glossary.cxx
//
//------------------------------------------------------------------------------
#include "Glossary.hxx"

#include "CRC64.hxx"

#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class GlossaryEntry
	//
	//--------------------------------------------------------------------------
	class GlossaryEntry {
	public:
		GlossaryEntry();
		GlossaryEntry( const char* pTop, const char* pEnd );
		GlossaryEntry( const GlossaryEntry& entry );
		~GlossaryEntry();
		GlossaryEntry& operator=( const GlossaryEntry& rhs );
	public:
		inline uint64_t	GetHash() const { return m_hash; }
		inline const char* GetAnchorTag() const { return m_anchorTag; }
		inline uint32_t GetLength() const { return m_length; }
		inline const char* GetTerm() const { return m_pTerm; }
	private:
		uint64_t	m_hash;				// 用語文字列から生成されたハッシュ値
		char		m_anchorTag[12];	// ハッシュ値の文字列表現（null 終端を含む）
		uint32_t	m_length;			// 用語文字列の長さ（バイト数）
		const char*	m_pTerm;			// 用語文字列のポインタ
	};

	GlossaryEntry::GlossaryEntry() : m_hash( 0 ),
									 m_length( 0 ),
									 m_pTerm( nullptr ) {
		m_anchorTag[0] = 0;
	}

	GlossaryEntry::GlossaryEntry( const char* pTop,
								  const char* pEnd ) : m_hash( 0 ),
													   m_length( pEnd - pTop ),
													   m_pTerm( pTop ) {
		m_hash = CRC64::Calc( pTop, pEnd, m_anchorTag );
	}

	GlossaryEntry::GlossaryEntry( const GlossaryEntry& entry ) : m_hash( entry.m_hash ),
																 m_length( entry.m_length ),
																 m_pTerm( entry.m_pTerm ) {
		::strcpy( this->m_anchorTag, entry.m_anchorTag );
	}

	GlossaryEntry::~GlossaryEntry() {
		//intentionally do nothing.
	}

	GlossaryEntry& GlossaryEntry::operator=( const GlossaryEntry& rhs ) {
		this->m_hash	= rhs.m_hash;
		this->m_length	= rhs.m_length;
		this->m_pTerm	= rhs.m_pTerm;
		::strcpy( this->m_anchorTag, rhs.m_anchorTag );
		return *this;
	}

	//--------------------------------------------------------------------------
	//
	// class Glossary::Impl
	//
	//--------------------------------------------------------------------------
	class Glossary::Impl {
	public:
		Impl();
		~Impl();
	public:
		void Register( const char* pTop, const char* pEnd );
		const char* GetAnchorTag( const char* pTerm,
								  const char* pTermEnd ) const;
		void SortIfNeed();
		void WriteWithTermLink( std::ostream& os,
								const char* pTop, const char* pEnd,
								uint32_t idx, WriteFunction* pWriteFunc ) const;
	private:
		std::vector<GlossaryEntry>	m_entries;
		bool m_sorted;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class Glossary
	//
	//--------------------------------------------------------------------------
	Glossary::Glossary() : m_pImpl( new Impl{} ) {
	}
	Glossary::~Glossary() {
		delete m_pImpl;
	}
	void Glossary::Register( const char* pTop, const char* pEnd ) {
		m_pImpl->Register( pTop, pEnd );
	}
	const char* Glossary::GetAnchorTag( const char* pTerm,
										const char* pTermEnd ) const {
		return m_pImpl->GetAnchorTag( pTerm, pTermEnd );
	}
	void Glossary::WriteWithTermLink( std::ostream& os,
									  const char* pTop,
									  const char* pEnd,
									  WriteFunction* pWriteFunc ) const {
		m_pImpl->SortIfNeed();
		m_pImpl->WriteWithTermLink( os, pTop, pEnd, 0, pWriteFunc );
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class Glossary::Impl
	//
	//--------------------------------------------------------------------------
	Glossary::Impl::Impl() : m_entries(),
							 m_sorted( false ) {
	}

	Glossary::Impl::~Impl() {
		m_entries.clear();
	}

	void Glossary::Impl::Register( const char* pTop, const char* pEnd ) {
		m_entries.emplace_back( pTop, pEnd );
		m_sorted = false;
	}

	const char* Glossary::Impl::GetAnchorTag( const char* pTerm,
											  const char* pTermEnd ) const {
		uint64_t hash = CRC64::Calc( pTerm, pTermEnd );
		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			if( m_entries[i].GetHash() == hash )
				return m_entries[i].GetAnchorTag();
		}
		return nullptr;
	}

	void Glossary::Impl::SortIfNeed() {
		if( !m_sorted ) {
			if( m_entries.empty() == false ) {
				std::sort( m_entries.begin(), m_entries.end(),
						   []( const GlossaryEntry& e1, const GlossaryEntry& e2 ) -> bool {
							   return e1.GetLength() > e2.GetLength();
						   } );
			}		
			m_sorted = true;
		}		
	}

	void Glossary::Impl::WriteWithTermLink( std::ostream& os,
											const char* pTop, const char* pEnd,
											uint32_t idx, WriteFunction* pWriteFunc ) const {
		for( ; idx < m_entries.size(); ++idx ) {
			auto& entry = m_entries[idx];
			auto pTerm1 = entry.GetTerm();
			auto length = entry.GetLength();
			while( pTop < pEnd ) {
				auto p = std::search( pTop, pEnd, pTerm1, pTerm1 + length );
				if( p == pEnd )
					break;
				if( pTop < p )
					this->WriteWithTermLink( os, pTop, p, idx + 1, pWriteFunc );
				os << "<a class='term' href='#" << entry.GetAnchorTag()  << "'>";
				pWriteFunc( os, p, p + length );
				os << "</a>";
				pTop = p + length;
			}
		}
		pWriteFunc( os, pTop, pEnd );
	}

} // namespace turnup

