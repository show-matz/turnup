
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

	class TocEntry;
	static bool CompareEntityPrefix( const TocEntry* p1,
									 const TocEntry* p2, const Config& cfg ); 


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
				int n = ::sprintf( pBuf, ".%u" + !!(lv==minLv), this->numbers[lv-1] );
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
		TocEntry( ToC::EntryT type, uint32_t lv,
				  const char* pTitle, const ChapterNumber& chapterNum );
		TocEntry( const TocEntry& entry );
		~TocEntry();
		TocEntry& operator=( const TocEntry& rhs );
	public:
		inline ToC::EntryT GetType() const { return m_type; }
		inline uint32_t GetLevel() const { return m_level; }
		inline uint64_t	GetHash() const { return m_hash; }
		inline const char* GetAnchorTag() const { return m_anchorTag; }
		inline const char* GetTitle() const { return m_pTitle; }
		inline const ChapterNumber& GetChapterNumber() const { return m_chapterNum; }
	public:
		const char* GetChapterPrefix( const Config& cfg, char* pBuf ) const;
	private:
		ToC::EntryT		m_type;				// 見出しか、図か、表か
		uint64_t		m_hash;				// タイトルから生成されたハッシュ値
		uint32_t		m_level;			// 1 - 6（見出し以外の場合は０）
		char			m_anchorTag[12];	// ハッシュ値の文字列表現（null 終端を含む）
		const char*		m_pTitle;			// タイトル文字列のポインタ
		ChapterNumber	m_chapterNum;		//
	};

	TocEntry::TocEntry() : m_type( ToC::EntryT::HEADER ),
						   m_hash( 0 ),
						   m_level( 0 ),
						   m_pTitle( nullptr ) {
		m_anchorTag[0] = 0;
	}

	TocEntry::TocEntry( ToC::EntryT type,
						uint32_t lv, const char* pTitle,
						const ChapterNumber& chapterNum ) : m_type( type ),
															m_hash( 0 ),
															m_level( lv ),
															m_pTitle( pTitle ),
															m_chapterNum( chapterNum ) {
		m_hash = CRC64::Calc( pTitle, nullptr, m_anchorTag );
	}

	TocEntry::TocEntry( const TocEntry& entry ) : m_type( entry.m_type ),
												  m_hash( entry.m_hash ),
												  m_level( entry.m_level ),
												  m_pTitle( entry.m_pTitle ),
												  m_chapterNum( entry.m_chapterNum ) {
		::strcpy( this->m_anchorTag, entry.m_anchorTag );
	}

	TocEntry::~TocEntry() {
		//intentionally do nothing.
	}

	TocEntry& TocEntry::operator=( const TocEntry& rhs ) {
		this->m_type	= rhs.m_type;
		this->m_hash	= rhs.m_hash;
		this->m_level	= rhs.m_level;
		this->m_pTitle	= rhs.m_pTitle;
		::strcpy( this->m_anchorTag, rhs.m_anchorTag );
		return *this;
	}

	const char* TocEntry::GetChapterPrefix( const Config& cfg, char* pBuf ) const {
		uint32_t minLv = cfg.minNumberingLv;
		uint32_t maxLv = cfg.maxNumberingLv;
		if( m_level < minLv || maxLv < m_level ) {
			*pBuf = 0;
			return pBuf;
		}
		return m_chapterNum.Format( minLv, std::min( maxLv, m_level ), pBuf );
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
		void RegisterImpl( ToC::EntryT type, uint32_t level, const char* pTitle );
		const char* GetAnchorTag( ToC::EntryT type,
								  const char* pTitle, const char* pTitleEnd ) const;
		bool GetEntryNumber( char* pBuf, EntryT type, const Config& cfg,
							 const char* pTitle, const char* pTitleEnd ) const;
	public:
		void WriteTOC( std::ostream& os, const Config& cfg,
					   uint32_t minLevel, uint32_t maxLevel ) const;
		void WriteTableFigureList( std::ostream& os,
								   const Config& cfg, ToC::EntryT type ) const;
	private:
		const TocEntry* FindEntry( ToC::EntryT type,
								   const char* pTitle, const char* pTitleEnd ) const;
		void GetFigureAndTablePrefix( char* pBuf, 
									  const TocEntry* pEntry, const Config& cfg ) const;
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
	void ToC::RegisterHeader( uint32_t level, const char* pTitle ) {
		m_pImpl->RegisterImpl( ToC::EntryT::HEADER, level, pTitle );
	}
	void ToC::RegisterTable( const char* pTitle ) {
		m_pImpl->RegisterImpl( ToC::EntryT::TABLE, 0, pTitle );
	}
	void ToC::RegisterFigure( const char* pTitle ) {
		m_pImpl->RegisterImpl( ToC::EntryT::FIGURE, 0, pTitle );
	}
	const char* ToC::GetAnchorTag( EntryT type,
								   const char* pTitle,
								   const char* pTitleEnd ) const {
		return m_pImpl->GetAnchorTag( type, pTitle, pTitleEnd );
	}
	bool ToC::GetEntryNumber( char* pBuf, EntryT type, const Config& cfg,
							  const char* pTitle, const char* pTitleEnd ) const {
		return m_pImpl->GetEntryNumber( pBuf, type, cfg, pTitle, pTitleEnd );
	}
	void ToC::WriteTOC( std::ostream& os, const Config& cfg,
						uint32_t minLevel, uint32_t maxLevel ) const {
		m_pImpl->WriteTOC( os, cfg, minLevel, maxLevel );
	}
	void ToC::WriteTableList( std::ostream& os, const Config& cfg ) const {
		m_pImpl->WriteTableFigureList( os, cfg, ToC::EntryT::TABLE );
	}
	void ToC::WriteFigureList( std::ostream& os, const Config& cfg ) const {
		m_pImpl->WriteTableFigureList( os, cfg, ToC::EntryT::FIGURE );
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

	void ToC::Impl::RegisterImpl( ToC::EntryT type, uint32_t level, const char* pTitle ) {
		//既存のエントリを走査し、新規エントリの章節番号を特定する
		//ToDo : g8fV6nATAgJ : こんなことしないでも、最後のエントリが知りたいそれなのでは？ → あとで試そう
		ChapterNumber chapterNum{};
		auto itr1 = m_entries.begin();
		auto itr2 = m_entries.end();
		for( ; itr1 != itr2; ++itr1 ) {
			if( itr1->GetType() == ToC::EntryT::HEADER )
				chapterNum.Increment( itr1->GetLevel() );
		}
		//ヘッダ追加の場合のみ、指定レベルをインクリメント（これによって下位の番号はリセットされる）
		//MEMO : ヘッダ以外の場合、所属チャプターのコピーが入るかたちになる。
		if( type == ToC::EntryT::HEADER )
			chapterNum.Increment( level );
		m_entries.emplace_back( type, level, pTitle, chapterNum );
	}

	const char* ToC::Impl::GetAnchorTag( ToC::EntryT type,
										 const char* pTitle,
										 const char* pTitleEnd ) const {
		auto pEntry = FindEntry( type, pTitle, pTitleEnd );
		return !pEntry ? nullptr : pEntry->GetAnchorTag();
	}

	bool ToC::Impl::GetEntryNumber( char* pBuf, EntryT type, const Config& cfg,
									const char* pTitle, const char* pTitleEnd ) const {
		auto pEntry = FindEntry( type, pTitle, pTitleEnd );
		if( !pEntry )
			return false;
		if( type == ToC::EntryT::HEADER ) {
			pEntry->GetChapterPrefix( cfg, pBuf );
			return true;
		}
		this->GetFigureAndTablePrefix( pBuf, pEntry, cfg );
		return true;
	}

	void ToC::Impl::WriteTOC( std::ostream& os, const Config& cfg,
							  uint32_t minLevel, uint32_t maxLevel ) const {
		uint32_t curLevel = minLevel;
		os << "<ul>" << std::endl;

		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const TocEntry& entry = m_entries[i];
			if( entry.GetType() != ToC::EntryT::HEADER )
				continue;
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
				entry.GetChapterPrefix( cfg, chapter );
				os << chapter;
			}
			os << entry.GetTitle() << "</a></li>" << std::endl;
		}
		for( ; minLevel < curLevel; --curLevel )
			os << "</ul>" << std::endl;
		os << "</ul>" << std::endl;
	}

	void ToC::Impl::WriteTableFigureList( std::ostream& os,
										  const Config& cfg, ToC::EntryT type ) const {
		os << "<ul>" << std::endl;

		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const TocEntry& entry = m_entries[i];
			if( entry.GetType() != type )
				continue;
			os << "<li><a href='#" << entry.GetAnchorTag() << "'>";
			char prefix[64];
			this->GetFigureAndTablePrefix( prefix, &entry, cfg );
			os << prefix << ' '  << entry.GetTitle() << "</a></li>" << std::endl;
		}
		os << "</ul>" << std::endl;
	}

	const TocEntry* ToC::Impl::FindEntry( ToC::EntryT type,
										  const char* pTitle, const char* pTitleEnd ) const {
		uint64_t hash = CRC64::Calc( pTitle, pTitleEnd );
		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const auto& entry = m_entries[i];
			if( entry.GetType() == type && entry.GetHash() == hash )
				return &entry;
		}
		return nullptr;
	}

	void ToC::Impl::GetFigureAndTablePrefix( char* pBuf, 
											 const TocEntry* pEntry, const Config& cfg ) const {
		const TocEntry* pTop = &(m_entries[0]);
		ToC::EntryT type = pEntry->GetType();

		uint32_t cnt = 0;
		for( const TocEntry* p = pEntry; pTop <= p; --p ) {
			if( p->GetType() != type )
				continue;
			if( CompareEntityPrefix( pEntry, p, cfg ) == false )
				break;
			++cnt;
		}
		const char* const tags[] = { "", "Table", "Figure" };
		const char* pTag = tags[static_cast<uint32_t>( type )];
		if( cfg.entityNumberingDepth == 0 ) {
			::strcpy( pBuf, pTag );
			pBuf += ::strlen( pTag );
			::sprintf( pBuf, ".%u", cnt );
		} else {
			const uint8_t* pNumbers = pEntry->GetChapterNumber().numbers;
			::strcpy( pBuf, pTag );
			pBuf += ::strlen( pTag );
			for( uint32_t i = 0; i < cfg.entityNumberingDepth; ++i ) {
				::sprintf( pBuf, ".%u", pNumbers[cfg.minNumberingLv - 1 + i] );
				pBuf += ::strlen( pBuf );
			}
			::sprintf( pBuf, "-%u", cnt );
		}
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static bool CompareEntityPrefix( const TocEntry* p1,
									 const TocEntry* p2, const Config& cfg ) {
		const uint8_t* pNumbers1 = p1->GetChapterNumber().numbers;
		const uint8_t* pNumbers2 = p2->GetChapterNumber().numbers;
		const uint32_t min = cfg.minNumberingLv - 1; 
		const uint32_t max = min + cfg.entityNumberingDepth;
		for( uint32_t i = min; i < max; ++i ) {
			if( pNumbers1[i] != pNumbers2[i] )
				return false;
		}
		return true;
	}


} // namespace turnup

