//------------------------------------------------------------------------------
//
// ToC.cxx
//
//------------------------------------------------------------------------------
#include "ToC.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "Config.hxx"
#include "CRC64.hxx"
#include "TextSpan.hxx"

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

	inline char GetCrcType( ToC::EntryT entryType ) {
		switch( entryType ) {
		case ToC::EntryT::HEADER:	return 'H';
		case ToC::EntryT::TABLE:	return 'T';
		case ToC::EntryT::FIGURE:	return 'F';
		}
		return 'X';		// 'X' means 'something other else'.
	}

	//--------------------------------------------------------------------------
	//
	// class TocEntry
	//
	//--------------------------------------------------------------------------
	class TocEntry {
	public:
		TocEntry();
		TocEntry( ToC::EntryT type, uint32_t lv,
				  const TextSpan& title, const ChapterNumber& chapterNum );
		TocEntry( const TocEntry& entry );
		~TocEntry();
		TocEntry& operator=( const TocEntry& rhs );
	public:
		inline ToC::EntryT GetType() const { return m_type; }
		inline uint32_t GetLevel() const { return m_level; }
		inline uint64_t	GetHash() const { return m_hash; }
		inline const char* GetAnchorTag() const { return m_anchorTag; }
		inline const TextSpan& GetTitle() const { return m_title; }
		inline const ChapterNumber& GetChapterNumber() const { return m_chapterNum; }
	public:
		const char* GetChapterPrefix( const Config& cfg, char* pBuf ) const;
	private:
		ToC::EntryT		m_type;				// 見出しか、図か、表か
		uint64_t		m_hash;				// タイトルから生成されたハッシュ値
		uint32_t		m_level;			// 1 - 6（見出し以外の場合は０）
		char			m_anchorTag[12];	// ハッシュ値の文字列表現（null 終端を含む）
		TextSpan		m_title;			// タイトル文字列を指す TextSpan
		ChapterNumber	m_chapterNum;		//
	};

	TocEntry::TocEntry() : m_type( ToC::EntryT::HEADER ),
						   m_hash( 0 ),
						   m_level( 0 ),
						   m_title( {} ) {
		m_anchorTag[0] = 0;
	}

	TocEntry::TocEntry( ToC::EntryT type, uint32_t lv,
						const TextSpan& title,
						const ChapterNumber& chapterNum ) : m_type( type ),
															m_hash( 0 ),
															m_level( lv ),
															m_title( title ),
															m_chapterNum( chapterNum ) {
		m_hash = CRC64::Calc( GetCrcType( type ),
							  title.Top(), title.End(), m_anchorTag );
	}

	TocEntry::TocEntry( const TocEntry& entry ) : m_type( entry.m_type ),
												  m_hash( entry.m_hash ),
												  m_level( entry.m_level ),
												  m_title( entry.m_title ),
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
		this->m_title	= rhs.m_title;
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
		bool RegisterImpl( ToC::EntryT type, uint32_t level, const TextSpan& title );
		bool RegisterLinkButton( bool bTop, const TextSpan& title );
		const char* GetAnchorTag( ToC::EntryT type,
								  const char* pTitle, const char* pTitleEnd ) const;
		bool GetEntryNumber( char* pBuf, EntryT type, const Config& cfg,
							 const char* pTitle, const char* pTitleEnd ) const;
	public:
		void WriteTocLinkIfNeed( std::ostream& os ) const;
	public:
		void WriteTOC( std::ostream& os,
					   DocumentInfo& docInfo,
					   uint32_t minLevel, uint32_t maxLevel ) const;
		void WriteTOC_X( std::ostream& os,
						 DocumentInfo& docInfo,
						 uint32_t minLevel, uint32_t maxLevel ) const;
		void WriteTableFigureList( std::ostream& os,
								   ToC::EntryT type, DocumentInfo& docInfo ) const;
	private:
		const TocEntry* FindEntry( ToC::EntryT type,
								   const char* pTitle, const char* pTitleEnd ) const;
		void GetFigureAndTablePrefix( char* pBuf, 
									  const TocEntry* pEntry, const Config& cfg ) const;
	private:
		std::vector<TocEntry>	m_entries;
		TextSpan				m_linkTarget;
		bool					m_bLinkTop;
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
	bool ToC::RegisterHeader( uint32_t level, const TextSpan& title ) {
		return m_pImpl->RegisterImpl( ToC::EntryT::HEADER, level, title );
	}
	bool ToC::RegisterTable( const TextSpan& title ) {
		return m_pImpl->RegisterImpl( ToC::EntryT::TABLE, 0, title );
	}
	bool ToC::RegisterFigure( const TextSpan& title ) {
		return m_pImpl->RegisterImpl( ToC::EntryT::FIGURE, 0, title );
	}
	bool ToC::RegisterLinkButton( bool bTop, const TextSpan& title ) {
		return m_pImpl->RegisterLinkButton( bTop, title );
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
	void ToC::WriteTocLinkIfNeed( std::ostream& os ) const {
		return m_pImpl->WriteTocLinkIfNeed( os );
	}
	void ToC::WriteTOC( std::ostream& os,
						DocumentInfo& docInfo, bool bFoldable,
						uint32_t minLevel, uint32_t maxLevel ) const {
		if( !bFoldable )
			m_pImpl->WriteTOC( os, docInfo, minLevel, maxLevel );
		else
			m_pImpl->WriteTOC_X( os, docInfo, minLevel, maxLevel );
	}
	void ToC::WriteTableList( std::ostream& os, DocumentInfo& docInfo ) const {
		m_pImpl->WriteTableFigureList( os, ToC::EntryT::TABLE, docInfo );
	}
	void ToC::WriteFigureList( std::ostream& os, DocumentInfo& docInfo ) const {
		m_pImpl->WriteTableFigureList( os, ToC::EntryT::FIGURE, docInfo );
	}

	//--------------------------------------------------------------------------
	//
	// template helper function for toc-x
	//
	//--------------------------------------------------------------------------
	template <typename T>
	T GetNextLevelEntry( T itr1, T itr2, uint32_t lv ) {
		for( ; itr1 != itr2; ++itr1 )
			if( (*itr1)->GetLevel() <= lv )
				break;
		return itr1;
	}

	template <typename T>
	void FooBar( T itr, std::ostream& os,
				 const char* pTag, DocumentInfo& docInfo ) {
		auto& cfg    = docInfo.Get<Config>();
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, pTag )
			<< "<a href='#" << (*itr)->GetAnchorTag() << "'>";
		if( cfg.bNumberingHeader ) {
			char chapter[32];
			(*itr)->GetChapterPrefix( cfg, chapter );
			os << chapter;
		}
		TextSpan tmp{ (*itr)->GetTitle() };
		tmp.WriteTo( os, docInfo, false ) << "</a></" << pTag << ">" << std::endl;
	}

	template <typename T>
	void RecursiveWriteEntries( T itr1, T itr2,
								std::ostream& os, DocumentInfo& docInfo ) {
		auto& styles = docInfo.Get<StyleStack>();
		while( itr1 != itr2 ) {
			auto itr = GetNextLevelEntry( itr1 + 1, itr2, (*itr1)->GetLevel() );
			if( (itr - itr1) == 1 ) {
				FooBar( itr1, os, "li", docInfo );
			} else {
				styles.WriteOpenTag( os, "details", nullptr, " open>" ) << std::endl;
				FooBar( itr1, os, "summary", docInfo );
				styles.WriteOpenTag( os, "blockquote" ) << std::endl;
				RecursiveWriteEntries( itr1 + 1, itr, os, docInfo );
				os << "</blockquote>" << std::endl;
				os << "</details>" << std::endl;
			}
			itr1 = itr;
		}
	} 

	//--------------------------------------------------------------------------
	//
	// implementation of class ToC::Impl
	//
	//--------------------------------------------------------------------------
	ToC::Impl::Impl() : m_entries(),
						m_linkTarget(),
						m_bLinkTop( false ) {
	}

	ToC::Impl::~Impl() {
		m_entries.clear();
	}

	bool ToC::Impl::RegisterImpl( ToC::EntryT type, 
								  uint32_t level, const TextSpan& title ) {
		//既存のエントリを走査し、タイトル名の重複をチェックしつつ新規エントリの章節番号を特定
		bool bDuplicated = false;
		ChapterNumber chapterNum{};
		auto itr1 = m_entries.begin();
		auto itr2 = m_entries.end();
		for( ; itr1 != itr2; ++itr1 ) {
			if( itr1->GetType() == type && title.IsEqual( itr1->GetTitle() ) )
				bDuplicated = true; 
			if( itr1->GetType() == ToC::EntryT::HEADER )
				chapterNum.Increment( itr1->GetLevel() );
		}
		//ヘッダ追加の場合のみ、指定レベルをインクリメント（これによって下位の番号はリセットされる）
		//MEMO : ヘッダ以外の場合、所属チャプターのコピーが入るかたちになる。
		if( type == ToC::EntryT::HEADER )
			chapterNum.Increment( level );
		m_entries.emplace_back( type, level, title, chapterNum );
		return !bDuplicated;
	}

	bool ToC::Impl::RegisterLinkButton( bool bTop, const TextSpan& title ) {
		bool ret = m_linkTarget.IsEmpty();
		m_bLinkTop   = bTop;
		m_linkTarget = title;
		return ret;
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

	void ToC::Impl::WriteTocLinkIfNeed( std::ostream& os ) const {
		if( m_linkTarget.IsEmpty() )
			return;
		const char* pLinkClass = m_bLinkTop ? "toc_button_top" : "toc_button_bottom";
		char linkTarget[12];
		CRC64::Calc( GetCrcType( ToC::EntryT::HEADER ),
					 m_linkTarget.Top(), m_linkTarget.End(), linkTarget );
		os << "<div class='" << pLinkClass  << "'><a href='#" << linkTarget << "'>" << std::endl
		   << "<svg xmlns='http://www.w3.org/2000/svg' width='40' height='40' viewBox='0 0 200 200'>" << std::endl
		   << "<defs><g id='line'>" << std::endl
		   << "  <circle cx='45' cy='50' r='15' fill='white' stroke='none' stroke-width='1' />" << std::endl
		   << "  <polyline fill='none' stroke='white' stroke-width='20' points=' 75.0,50.0 170.0,50.0' />" << std::endl
		   << "</g></defs>" << std::endl
		   << "<rect x='0' y='0' width='200' height='200' rx='30' ry='30' fill='lightgray' stroke='none' stroke-width='1' />" << std::endl
		   << "<use xlink:href='#line' x='0' y='0' />" << std::endl
		   << "<use xlink:href='#line' x='0' y='50' />" << std::endl
		   << "<use xlink:href='#line' x='0' y='100' />" << std::endl
		   << "</svg>" << std::endl
		   << "</a></div>" << std::endl;
	}

	void ToC::Impl::WriteTOC( std::ostream& os,
							  DocumentInfo& docInfo,
							  uint32_t minLevel, uint32_t maxLevel ) const {
		auto& cfg    = docInfo.Get<Config>();
		auto& styles = docInfo.Get<StyleStack>();
		uint32_t curLevel = minLevel;
		styles.WriteOpenTag( os, "ul" ) << std::endl;

		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const TocEntry& entry = m_entries[i];
			if( entry.GetType() != ToC::EntryT::HEADER )
				continue;
			uint32_t lv = entry.GetLevel();
			if( lv < minLevel || maxLevel < lv )
				continue;
			for( ; curLevel < lv; ++curLevel )
				styles.WriteOpenTag( os, "ul" ) << std::endl;
			for( ; lv < curLevel; --curLevel )
				os << "</ul>" << std::endl;
			styles.WriteOpenTag( os, "li" )
						<< "<a href='#" << entry.GetAnchorTag() << "'>";
			if( cfg.bNumberingHeader ) {
				char chapter[32];
				entry.GetChapterPrefix( cfg, chapter );
				os << chapter;
			}
			TextSpan tmp{ entry.GetTitle() };
			tmp.WriteTo( os, docInfo, false ) << "</a></li>" << std::endl;
		}
		for( ; minLevel < curLevel; --curLevel )
			os << "</ul>" << std::endl;
		os << "</ul>" << std::endl;
	}

	void ToC::Impl::WriteTOC_X( std::ostream& os,
								DocumentInfo& docInfo,
								uint32_t minLevel, uint32_t maxLevel ) const {

		//対象となるエントリの const ポインタのみ一時コンテナに回収
		std::vector<const TocEntry*> entries;
		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const TocEntry& e = m_entries[i];
			if( e.GetType() == ToC::EntryT::HEADER ) {
				uint32_t lv = e.GetLevel();
				if( minLevel <= lv && lv <= maxLevel )
					entries.push_back( &e );
			}
		}
		//MEMO : 妥当かどうかわからないが、上記回収結果のシーケンスは必ず minLevel の
		//       エントリから始まり、レベルに跳躍はないと想定する。

		auto& styles = docInfo.Get<StyleStack>(); {
			TextSpan listitm{ "li" };
			TextSpan details{ "details" };
			TextSpan summary{ "summary" };
			TextSpan bkquote{ "blockquote" };
			TextSpan clstocx{ "class='tocx'" };
			styles.PushStyle( listitm, clstocx );
			styles.PushStyle( details, clstocx );
			styles.PushStyle( summary, clstocx );
			styles.PushStyle( bkquote, clstocx );
			RecursiveWriteEntries( entries.begin(), entries.end(), os, docInfo );
			styles.PopStyle( listitm );
			styles.PopStyle( details );
			styles.PopStyle( summary );
			styles.PopStyle( bkquote );
		}
	}

	void ToC::Impl::WriteTableFigureList( std::ostream& os,
										  ToC::EntryT type, DocumentInfo& docInfo ) const {
		auto& cfg    = docInfo.Get<Config>();
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "ul" ) << std::endl;

		for( uint32_t i = 0; i < m_entries.size(); ++i ) {
			const TocEntry& entry = m_entries[i];
			if( entry.GetType() != type )
				continue;
			styles.WriteOpenTag( os, "li" )
							<< "<a href='#" << entry.GetAnchorTag() << "'>";
			char prefix[64];
			this->GetFigureAndTablePrefix( prefix, &entry, cfg );
			os << prefix << ' ';
			TextSpan tmp{ entry.GetTitle() };
			tmp.WriteTo( os, docInfo, false ) << "</a></li>" << std::endl;
		}
		os << "</ul>" << std::endl;
	}

	const TocEntry* ToC::Impl::FindEntry( ToC::EntryT type,
										  const char* pTitle, const char* pTitleEnd ) const {
		uint64_t hash = CRC64::Calc( GetCrcType( type ), pTitle, pTitleEnd );
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

