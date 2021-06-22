//------------------------------------------------------------------------------
//
// ToC.cxx
//
//------------------------------------------------------------------------------
#include "ToC.hxx"

#include "Config.hxx"
#include "CRC64.hxx"

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>

namespace turnup {

	struct ChapterNumber {
	public:
		ChapterNumber() {
			for( int i = 0; i < 6; ++i )
				numbers[i] = 0;
		}
	public:
		void Increment( uint32_t lv ) {
			auto x = lv - 1;
			this->numbers[x] += 1;
			for( ++x; x < 6; ++x )
				this->numbers[x] = 0;
		} 
		const char* Format( uint32_t minLv,
							uint32_t maxLv, char* pBuf ) const {
			for( uint32_t lv = minLv; lv <= maxLv; ++lv ) {
				int n = ::sprintf( pBuf, "%u.", this->numbers[lv-1] );
				pBuf += n;
			}
			*pBuf++ = ' ';
			*pBuf   = 0;
			return pBuf;
		}
	public:
		uint8_t numbers[6];
	};

	//--------------------------------------------------------------------------
	//
	// class TocEntry
	//
	//--------------------------------------------------------------------------
	class TocEntry {
	public:
		TocEntry();
		TocEntry( uint32_t lv, const char* pTitle,
				  const ChapterNumber& chapterNum );
		TocEntry( const TocEntry& entry );
		~TocEntry();
		TocEntry& operator=( const TocEntry& rhs );
	public:
		inline uint32_t GetLevel() const { return m_level; }
		inline uint64_t	GetHash() const { return m_hash; }
		inline const char* GetAnchorTag() const { return m_anchorTag; }
		inline const char* GetTitle() const { return m_pTitle; }
		inline const char* GetChapterNumber( uint32_t minLv,
											 uint32_t maxLv, char* pBuf ) const {
			if( m_level < minLv || maxLv < m_level ) {
				*pBuf = 0;
				return pBuf;
			}
			return m_chapterNum.Format( minLv, std::min( maxLv, m_level ), pBuf );
		}
	private:
		uint64_t		m_hash;				// タイトルから生成されたハッシュ値
		uint32_t		m_level;			// 1 - 6
		char			m_anchorTag[12];	// ハッシュ値の文字列表現（null 終端を含む）
		const char*		m_pTitle;			// タイトル文字列のポインタ
		ChapterNumber	m_chapterNum;		//
	};

	TocEntry::TocEntry() : m_hash( 0 ),
						   m_level( 0 ),
						   m_pTitle( nullptr ) {
		m_anchorTag[0] = 0;
	}

	TocEntry::TocEntry( uint32_t lv, const char* pTitle,
						const ChapterNumber& chapterNum ) : m_hash( 0 ),
															m_level( lv ),
															m_pTitle( pTitle ),
															m_chapterNum( chapterNum ) {
		m_hash = CRC64::Calc( pTitle, nullptr, m_anchorTag );
	}

	TocEntry::TocEntry( const TocEntry& entry ) : m_hash( entry.m_hash ),
												  m_level( entry.m_level ),
												  m_pTitle( entry.m_pTitle ),
												  m_chapterNum( entry.m_chapterNum ) {
		::strcpy( this->m_anchorTag, entry.m_anchorTag );
	}

	TocEntry::~TocEntry() {
		//intentionally do nothing.
	}

	TocEntry& TocEntry::operator=( const TocEntry& rhs ) {
		this->m_hash	= rhs.m_hash;
		this->m_level	= rhs.m_level;
		this->m_pTitle	= rhs.m_pTitle;
		::strcpy( this->m_anchorTag, rhs.m_anchorTag );
		return *this;
	}

	//--------------------------------------------------------------------------
	//
	// class ToC::Impl
	//
	//--------------------------------------------------------------------------
	class ToC::Impl {
	public:
		Impl();
		~Impl();
	public:
		void Register( uint32_t level, const char* pTitle );
		const char* GetAnchorTag( const char* pTitle,
								  const char* pTitleEnd ) const;
		bool GetChapterNumber( char* pBuf, const Config& cfg,
							   const char* pTitle, const char* pTitleEnd ) const;
	public:
		void WriteTOC( std::ostream& os, const Config& cfg,
					   uint32_t minLevel, uint32_t maxLevel ) const;
	private:
		std::vector<TocEntry>	m_entries;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class ToC
	//
	//--------------------------------------------------------------------------
	ToC::ToC() : m_pImpl( new Impl{} ) {
	}
	ToC::~ToC() {
		delete m_pImpl;
	}
	void ToC::Register( uint32_t level, const char* pTitle ) {
		m_pImpl->Register( level, pTitle );
	}
	const char* ToC::GetAnchorTag( const char* pTitle,
								   const char* pTitleEnd ) const {
		return m_pImpl->GetAnchorTag( pTitle, pTitleEnd );
	}
	bool ToC::GetChapterNumber( char* pBuf, const Config& cfg,
								const char* pTitle, const char* pTitleEnd ) const {
		return m_pImpl->GetChapterNumber( pBuf, cfg, pTitle, pTitleEnd );
	}
	void ToC::WriteTOC( std::ostream& os, const Config& cfg,
						uint32_t minLevel, uint32_t maxLevel ) const {
		m_pImpl->WriteTOC( os, cfg, minLevel, maxLevel );
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class ToC::Impl
	//
	//--------------------------------------------------------------------------
	ToC::Impl::Impl() : m_entries() {
	}

	ToC::Impl::~Impl() {
		m_entries.clear();
	}

	void ToC::Impl::Register( uint32_t level, const char* pTitle ) {
		//既存のエントリを走査し、新規エントリの章節番号を特定する
		ChapterNumber chapterNum{};
		auto itr1 = m_entries.begin();
		auto itr2 = m_entries.end();
		for( ; itr1 != itr2; ++itr1 ) {
			chapterNum.Increment( itr1->GetLevel() );
		}
		chapterNum.Increment( level );
		m_entries.emplace_back( level, pTitle, chapterNum );
	}

	const char* ToC::Impl::GetAnchorTag( const char* pTitle,
										 const char* pTitleEnd ) const {
		uint64_t hash = CRC64::Calc( pTitle, pTitleEnd );
		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			if( m_entries[i].GetHash() == hash )
				return m_entries[i].GetAnchorTag();
		}
		return nullptr;
	}

	bool ToC::Impl::GetChapterNumber( char* pBuf, const Config& cfg,
									  const char* pTitle,
									  const char* pTitleEnd ) const {
		uint64_t hash = CRC64::Calc( pTitle, pTitleEnd );
		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			if( m_entries[i].GetHash() == hash ) {
				m_entries[i].GetChapterNumber( cfg.minNumberingLv,
											   cfg.maxNumberingLv, pBuf );
				return true;
			}
		}
		return false;
	}

	void ToC::Impl::WriteTOC( std::ostream& os, const Config& cfg,
							  uint32_t minLevel, uint32_t maxLevel ) const {
		uint32_t curLevel = minLevel;
		os << "<ul>" << std::endl;

		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const TocEntry& entry = m_entries[i];
			uint32_t lv = entry.GetLevel();
			if( lv < minLevel || maxLevel < lv )
				continue;
			for( ; curLevel < lv; ++curLevel )
				os << "<ul>" << std::endl;
			for( ; lv < curLevel; --curLevel )
				os << "</ul>" << std::endl;
			os << "<li><a href='#" << entry.GetAnchorTag() << "'>";
			if( cfg.bNumberingHeader ) {
				char chapter[32];
				entry.GetChapterNumber( cfg.minNumberingLv,
										cfg.maxNumberingLv, chapter );
				os << chapter;
			}
			os << entry.GetTitle() << "</a></li>" << std::endl;
		}
		for( ; minLevel < curLevel; --curLevel )
			os << "</ul>" << std::endl;
		os << "</ul>" << std::endl;
	}

} // namespace turnup

