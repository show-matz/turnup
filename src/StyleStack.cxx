//------------------------------------------------------------------------------
//
// StyleStack.cxx
//
//------------------------------------------------------------------------------
#include "StyleStack.hxx"

#include "TextSpan.hxx"

#include <string.h>
#include <ostream>
#include <vector>
#include <utility>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class StyleNode
	//
	//--------------------------------------------------------------------------
	class StyleNode {
	public:
		StyleNode() : m_style( {} ),
					  m_pNext( nullptr ) {
		}
		StyleNode( const TextSpan& style,
					StyleNode* pNext = nullptr ) : m_style( style ),
												   m_pNext( pNext ) {
		}
		StyleNode( const StyleNode& entry ) = delete;
		~StyleNode() {
			delete m_pNext;
		}
	public:
		inline void SetNext( StyleNode* pNext ) { m_pNext = pNext; }
		inline StyleNode* GetNext() const { return m_pNext; }
		inline const TextSpan* GetStyle() const { return &m_style; }
	private:
		TextSpan	m_style;
		StyleNode*	m_pNext;
	};

	//--------------------------------------------------------------------------
	//
	// class StyleStack::Impl
	//
	//--------------------------------------------------------------------------
	class StyleStack::Impl {
	private:
		typedef std::pair<TextSpan,StyleNode*>	StackEntry;
		typedef std::vector<StackEntry>			StackEntryList;
	public:
		Impl();
		~Impl();
	public:
		void PushStyle( const TextSpan& tagName, const TextSpan& style );
		bool PopStyle( const TextSpan& tagName );
		const TextSpan* GetCurrentStyle( const char* pTagName ) const;
	private:
		StackEntryList	m_entries;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class StyleStack
	//
	//--------------------------------------------------------------------------
	StyleStack::StyleStack() : m_pImpl( nullptr ) {
	}

	StyleStack::~StyleStack() {
		delete m_pImpl;
	}

	void StyleStack::PushStyle( const TextSpan& tagName, const TextSpan& style ) {
		if( !m_pImpl )
			m_pImpl = new Impl{};
		return m_pImpl->PushStyle( tagName, style );
	}

	bool StyleStack::PopStyle( const TextSpan& tagName ) {
		if( !m_pImpl )
			return false;
		return m_pImpl->PopStyle( tagName );
	}

	std::ostream& StyleStack::WriteOpenTag( std::ostream& os, const char* pTagName,
											const char* pDefault, const char* pTail ) const {
		const TextSpan* pStyle = nullptr;
		if( !pTail )	pTail    = ">";
		if( !pDefault )	pDefault = "";
		if( m_pImpl )
			pStyle = m_pImpl->GetCurrentStyle( pTagName );
		if( !pStyle )
			os << '<' << pTagName << pDefault << pTail;
		else {
			os << '<' << pTagName << ' ';
			pStyle->WriteSimple( os ) << pTail;
		}
		return os;
	}


	//--------------------------------------------------------------------------
	//
	// implementation of class StyleStack::Impl
	//
	//--------------------------------------------------------------------------
	StyleStack::Impl::Impl() : m_entries( {} ) {
	}

	StyleStack::Impl::~Impl() {
		for( StackEntry& entry : m_entries ) {
			delete entry.second;
			entry.second = nullptr;;
		}
		m_entries.clear();
	}

	void StyleStack::Impl::PushStyle( const TextSpan& tagName, const TextSpan& style ) {
		//エントリ一覧が空なら追加して終了
		if( m_entries.empty() ) {
			m_entries.emplace_back( tagName, new StyleNode{ style } );
			return;
		}
		//同一タグ名のエントリを検索
		//MEMO : そんなに多くの種類を登録するとは思えないので、ひとまず線形探索にしておく
		auto itr1 = m_entries.begin();
		auto itr2 = m_entries.end();
		for( ; itr1 != itr2; ++itr1 ) {
			if( itr1->first.IsEqual( tagName ) )
				break;
		}
		//見つからなければ末尾に追加
		if( itr1 == itr2 ) {
			m_entries.emplace_back( tagName, new StyleNode{ style } );

		//見つかった場合、チェイン先頭に push
		} else {
			StyleNode* pNode = new StyleNode{ style, itr1->second };
			itr1->second = pNode;
		}
	}

	bool StyleStack::Impl::PopStyle( const TextSpan& tagName ) {
		//同一タグ名のエントリを検索
		//MEMO : そんなに多くの種類を登録するとは思えないので、ひとまず線形探索にしておく
		auto itr1 = m_entries.begin();
		auto itr2 = m_entries.end();
		for( ; itr1 != itr2; ++itr1 ) {
			if( itr1->first.IsEqual( tagName ) )
				break;
		}
		//見つからなければ false 復帰
		if( itr1 == itr2 )
			return false;

		//見つかった場合、チェイン先頭を取り出して削除
		StyleNode* pNode = itr1->second;
		itr1->second = pNode->GetNext();
		pNode->SetNext( nullptr );
		delete pNode;

		//エントリのスタックが空になった場合、エントリも除去
		if( itr1->second == nullptr )
			m_entries.erase( itr1 );

		return true;
	}

	const TextSpan* StyleStack::Impl::GetCurrentStyle( const char* pTagName ) const {
		//エントリ一覧が空なら nullptr 復帰
		if( m_entries.empty() )
			return nullptr;

		//同一タグ名のエントリを検索
		//MEMO : そんなに多くの種類を登録するとは思えないので、ひとまず線形探索にしておく
		auto itr1 = m_entries.begin();
		auto itr2 = m_entries.end();
		for( ; itr1 != itr2; ++itr1 ) {
			if( itr1->first.IsEqual( pTagName ) )
				break;
		}
		//見つからなければ nullptr 復帰
		if( itr1 == itr2 )
			return nullptr;
		
		return itr1->second->GetStyle();
	}


} // namespace turnup

