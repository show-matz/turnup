//------------------------------------------------------------------------------
//
// TextSpan.cxx
//
//------------------------------------------------------------------------------
#include "TextSpan.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "Config.hxx"
#include "ToC.hxx"
#include "Footnotes.hxx"
#include "Glossary.hxx"
#include "Utilities.hxx"

#include <assert.h>
#include <string.h>
#include <iostream>
#include <algorithm>

namespace turnup {

	//MEMO : variadic template が使えない環境も考慮に入れて...
	template <typename T>
	inline T min3( T t1, T t2, T t3 ) {
		return std::min( t1, std::min( t2, t3 ) );
	}

	inline bool IsTopOfSpaceRequiredOperator( const char* p1,
											  const char* p2, const TextSpan& whole ) {
		// 内側に空白類文字があれば該当しないと判断
		if( IsSpaceForward( p2 ) )
			return false;
		// 先頭であるか、または手前に空白類文字があれば該当
		return ( p1 == whole.Top() || IsSpaceBackward( p1 - 1 ) );
	}
	inline bool IsEndOfSpaceRequiredOperator( const char* p1,
											  const char* p2, const TextSpan& whole ) {
		// 内側に空白類文字があれば該当しないと判断
		if( IsSpaceBackward( p1 - 1 ) )
			return false;
		// 末尾であるか、または直後に空白類文字があれば該当
		return ( p2 == whole.End() || IsSpaceForward( p2 ) );
	}



	static void WriteTextSpanImp( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink );
	// ^xxx^ 形式（sup）を処理する
	static const char* FindSupPattern( const char* pTop, const char* pEnd, 
									   const char*& endPos, const TextSpan& whole );
	static void OperateSup( std::ostream& os, const TextSpan& whole,
							const char* pTop, const char* pEnd,
							DocumentInfo& docInfo, bool bTermLink );
	// ~xxx~ 形式（sub）を処理する
	static const char* FindSubPattern( const char* pTop, const char* pEnd, 
									   const char*& endPos, const TextSpan& whole );
	static void OperateSub( std::ostream& os, const TextSpan& whole,
							const char* pTop, const char* pEnd,
							DocumentInfo& docInfo, bool bTermLink );
	// ==xxx== 形式（mark）を処理する
	static const char* FindMarkPattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole );
	static void OperateMark( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink );
	// @{{STYLES}{CONTENTS}} / @((STYLES)(CONTENTS)) 形式を処理する
	static const char* FindStylesPatternImpl( const char* pPattern,
											  const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole );
	static const char* FindStylesPattern1( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole );
	static const char* FindStylesPattern2( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole );
	static void OperateStyles( std::ostream& os, const TextSpan& whole,
							   const char* pTop, const char* pEnd,
							   DocumentInfo& docInfo, bool bTermLink );
	// [label](URL) 形式のリンクを処理する
	static const char* FindLinkPattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole );
	static void OperateLink( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink );
	static void OperateInnerLink( std::ostream& os, ToC::EntryT type,
								  const char* pLbl1, const char* pLbl2,
								  const char* pURL1, const char* pURL2, DocumentInfo& docInfo );
	static const char* FindLinkLabelPlaceHolder( const char* pLabelTop,
												 const char* pLabelEnd );
	// <br> 形式の（一部の許可された）HTMLタグを処理する
	static const char* FindTagsPattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole );
	static void OperateTags( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink );
	// `code` 形式を処理する
	static const char* FindCodePattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole );
	static void OperateCode( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink );
	// $formula$ 形式を処理する
	static const char* FindMathJaxPattern( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole );
	static void OperateMathJax( std::ostream& os, const TextSpan& whole,
								const char* pTop, const char* pEnd,
								DocumentInfo& docInfo, bool bTermLink );
	// ![alt](url) 形式を処理する
	static const char* FindImagePattern( const char* pTop, const char* pEnd, 
										 const char*& endPos, const TextSpan& whole );
	static void OperateImage( std::ostream& os, const TextSpan& whole,
							  const char* pTop, const char* pEnd,
							  DocumentInfo& docInfo, bool bTermLink );
	// ~~string~~ 形式を処理する
	static const char* FindStrikePattern( const char* pTop, const char* pEnd, 
										  const char*& endPos, const TextSpan& whole );
	static void OperateStrike( std::ostream& os, const TextSpan& whole,
							   const char* pTop, const char* pEnd,
							   DocumentInfo& docInfo, bool bTermLink );
	// ___string___, ***string*** 　形式による強調を処理する
	static const char* FindEmphasis3Pattern1( const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole );
	static const char* FindEmphasis3Pattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole );
	static void OperateEmphasis3( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink );
	// __string__, **string** 　形式による強調を処理する
	static const char* FindEmphasis2Pattern1( const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole );
	static const char* FindEmphasis2Pattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole );
	static void OperateEmphasis2( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink );
	// _string_, *string* 　形式による強調を処理する
	static const char* FindEmphasis1Pattern1( const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole );
	static const char* FindEmphasis1Pattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole );
	static void OperateEmphasis1( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink );
	// {{fn:string}} 形式の脚注を処理する
	static const char* FindFootnotePattern( const char* pTop, const char* pEnd, 
											const char*& endPos, const TextSpan& whole );
	static void OperateFootnote( std::ostream& os, const TextSpan& whole,
								 const char* pTop, const char* pEnd,
								 DocumentInfo& docInfo, bool bTermLink );
	// {{fn(TAG):string}} 形式の脚注を処理する
	static const char* FindFootnotePattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole );
	static void OperateFootnote2( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink );
	// #((ANCHOR)) 形式を処理する
	static const char* FindAnchorPatternImpl( const char* pPattern,
											  const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole );
	static const char* FindAnchorPattern1( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole );
	static const char* FindAnchorPattern2( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole );
	static void OperateAnchor( std::ostream& os, const TextSpan& whole,
							   const char* pTop, const char* pEnd,
							   DocumentInfo& docInfo, bool bTermLink );


	static const char* FindSpaceRequiredPatternImpl( const char* pPattern, uint32_t len,
													 const char* pTop, const char* pEnd, 
													 const char*& endPos, const TextSpan& whole );
	static void WriteWithTermLink( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink );


	typedef const char* Finder( const char* pTop, const char* pEnd, 
								const char*& endPos, const TextSpan& whole );
	typedef void Writer( std::ostream& os, const TextSpan& whole,
						 const char* pTop, const char* pEnd,
						 DocumentInfo& docInfo, bool bTermLink );
	static const struct {
		Finder*     finder;
		Writer*     writer;
	} s_operators[] = {
		{ FindImagePattern,      OperateImage },		// ![alt](url)
		{ FindEmphasis3Pattern1, OperateEmphasis3 },	// ___emphasis___
		{ FindEmphasis2Pattern1, OperateEmphasis2 },	// __emphasis__
		{ FindEmphasis1Pattern1, OperateEmphasis1 },	// _emphasis_
		{ FindTagsPattern,       OperateTags },			// <br>
		{ FindMarkPattern,       OperateMark },			// ==mark==
		{ FindStylesPattern1,    OperateStyles },		// @{{styles}{contents}}
		{ FindStylesPattern2,    OperateStyles },		// @((styles)(contents))
		{ FindAnchorPattern1,    OperateAnchor },		// #((anchor))
		{ FindAnchorPattern2,    OperateAnchor },		// #{{anchor}}
		{ FindLinkPattern,       OperateLink },			// [label](url)
		{ FindSupPattern,        OperateSup },			// ^sup^
		{ FindSubPattern,        OperateSub },			// ~sub~
		{ FindEmphasis3Pattern2, OperateEmphasis3 },	// ***emphasis***
		{ FindEmphasis2Pattern2, OperateEmphasis2 },	// **emphasis**
		{ FindEmphasis1Pattern2, OperateEmphasis1 },	// *emphasis*
		{ FindCodePattern,       OperateCode },			// `code`
		{ FindMathJaxPattern,    OperateMathJax },		// $formula$
		{ FindFootnotePattern,   OperateFootnote },		// {{fn:footnote}}
		{ FindFootnotePattern2,  OperateFootnote2 },	// {{fn(TAG):footnote}}
		{ FindStrikePattern,     OperateStrike },		// ~~strike~~
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class TextSpan
	//
	//--------------------------------------------------------------------------
	TextSpan::TextSpan() : m_pTop( nullptr ), m_pEnd( nullptr ) {
	}

	TextSpan::TextSpan( const char* p ) : m_pTop( p ),
										  m_pEnd( nullptr ) {
		m_pEnd = p + ::strlen( p );
	}

	TextSpan::TextSpan( const char* pTop, const char* pEnd ) : m_pTop( pTop ),
															   m_pEnd( pEnd ) {
	}

	TextSpan::~TextSpan() {
	}

	bool TextSpan::IsEmpty() const {
		//ToDo : std::all_of() 利用に置き換えられる？
		if( m_pTop == m_pEnd )
			return true;
		for( auto p = m_pTop; p < m_pEnd; ++p ) {
			if( *p != ' ' && *p != 0x09 )
				return false;
		}
		return true;
	}

	TextSpan TextSpan::Trim() const {
		auto p1 = m_pTop;
		auto p2 = m_pEnd;
		while( p1 < p2 && (*p1 == ' ' || *p1 == 0x09) )
			++p1;
		while( p1 < p2 && (p2[-1] == ' ' || p2[-1] == 0x09) )
			--p2;
		return TextSpan{ p1, p2 };
	}

	TextSpan TextSpan::TrimHead() const {
		auto p1 = m_pTop;
		while( p1 < m_pEnd && (*p1 == ' ' || *p1 == 0x09) )
			++p1;
		return TextSpan{ p1, m_pEnd };
	}

	TextSpan TextSpan::TrimTail() const {
		auto p2 = m_pEnd;
		while( m_pTop < p2 && (p2[-1] == ' ' || p2[-1] == 0x09) )
			--p2;
		return TextSpan{ m_pTop, p2 };
	}

	bool TextSpan::IsMatch( const char* pHead,
							TextSpan& item, const char* pTail ) const {
		auto p1 = m_pTop;
		auto p2 = m_pEnd;
		for( ; p1 < p2 && !!*pHead; ++p1, ++pHead ) {
			if( *pHead != *p1 )
				return false;
		}
		if( !!*pHead )
			return false;
		item.Top() = p1;
		item.End() = p2;
		uint32_t len = ::strlen( pTail );
		if( item.ByteLength() < len )
			return false;
		if( !!::strncmp( m_pEnd - len, pTail, len ) )
			return false;
		item.End() = m_pEnd - len;
		return true;
	}

	bool TextSpan::IsMatch( const char* pHead, TextSpan& item1,
							const char* pDelim, TextSpan& item2, const char* pTail ) const {
		TextSpan tmp;
		if( this->IsMatch( pHead, tmp, pTail ) == false )
			return false;
		auto len = ::strlen( pDelim );
		auto p = std::search( tmp.Top(), tmp.End(), pDelim, pDelim + len );
		if( p == tmp.End() )
			return false;
		item1 = TextSpan{ tmp.Top(), p };
		item2 = TextSpan{ p + len, tmp.End() };
		return true;
	}

	bool TextSpan::IsEqual( const TextSpan& other ) const {
		if( this->ByteLength() != other.ByteLength() )
			return false;
		return std::equal( this->Top(), this->End(), other.Top() );
	}

	bool TextSpan::IsEqual( const char* p ) const {
		auto p1 = m_pTop;
		auto p2 = m_pEnd;
		for( ; p1 < p2 && !!*p; ++p1, ++p ) {
			if( *p != *p1 )
				return false;
		}
		return !*p && (p1 == p2);
	}

	bool TextSpan::BeginWith( const char* p ) const {
		auto p1 = m_pTop;
		auto p2 = m_pEnd;
		for( ; p1 < p2 && !!*p; ++p1, ++p ) {
			if( *p != *p1 )
				return false;
		}
		return !*p;
	}

	bool TextSpan::EndWith( const char* p ) const {
		uint32_t len = ::strlen( p );
		if( this->ByteLength() < len )
			return false;
		return !::strncmp( m_pEnd - len, p, len );
	}

	uint32_t TextSpan::CountTopOf( char ch ) const {
		uint32_t cnt = 0;
		for( auto p = m_pTop; p < m_pEnd; ++p, ++cnt ) {
			if( *p != ch )
				break;
		}
		return cnt;
	}

	bool TextSpan::Convert( uint32_t& val ) const {
		uint32_t v = 0;
		for( auto p = m_pTop; p < m_pEnd; ++p ) {
			int n = *p - '0';
			if( n < 0 || 9 < n )
				return false;
			v = (v * 10) + n; 
		}
		val = v;
		return true;
	}

	TextSpan TextSpan::CutNextToken() {
		if( this->IsEmpty() )
			return *this;
		auto pTop = m_pTop;
		auto p = std::find_if( pTop, m_pEnd,
							   []( char c ) { return c == ' ' || c == 0x09; } );
		this->Chomp( p - pTop, 0 );
		return TextSpan{ pTop, p };
	}

	TextSpan TextSpan::CutNextToken( char delimiter ) {
		if( this->IsEmpty() )
			return *this;
		auto pTop = m_pTop;
		auto p = std::find( pTop, m_pEnd, delimiter );
		this->Chomp( p - pTop, 0 );
		return TextSpan{ pTop, p };
	}

	TextSpan& TextSpan::Chomp( uint32_t head, uint32_t tail ) {
		if( this->ByteLength() <= (head+tail) )
			this->Clear();
		else {
			m_pTop += head;
			m_pEnd -= tail;
		}
		return *this;
	}

	void TextSpan::Clear() {
		m_pTop = m_pEnd;
	}

	std::ostream& TextSpan::WriteTo( std::ostream& os,
									 DocumentInfo& docInfo, bool bTermLink ) const {
		WriteTextSpanImp( os, *this, this->Top(), this->End(), docInfo, bTermLink );
		return os;
	}

	std::ostream& TextSpan::WriteSimple( std::ostream& os ) const {
		WriteWithEscape( os, this->Top(), this->End() );
		return os;
	}

	/*
	 * < を &lt; に
	 * > を &gt; に
	 * & を &amp; に
	 */
	void TextSpan::WriteWithEscape( std::ostream& os,
									const char* pTop, const char* pEnd ) {
		auto i1 = std::find( pTop, pEnd, '&' );
		auto i2 = std::find( pTop, pEnd, '<' );
		auto i3 = std::find( pTop, pEnd, '>' );

		while( pTop < pEnd ) {
			auto top = min3( i1, i2, i3 );
			if( top == pEnd ) {
				os.write( pTop, pEnd - pTop );
				pTop = pEnd;
				break;
			}
			switch( *top ) {
			case '&':	os.write( pTop, top - pTop );
						os << "&amp;";
						pTop = top + 1;
						i1 = std::find( pTop, pEnd, '&' );
						break;
			case '<':	os.write( pTop, top - pTop );
						os << "&lt;";
						pTop = top + 1;
						i2 = std::find( pTop, pEnd, '<' );
						break;
			case '>':	os.write( pTop, top - pTop );
						os << "&gt;";
						pTop = top + 1;
						i3 = std::find( pTop, pEnd, '>' );
						break;
			}
		}
	}

	void TextSpan::DestructureToken( TextSpan data,
									 bool(*callback)(TextSpan, void*), void* pOpaque ) {
		while( true ) {
			data = data.Trim();
			if( data.IsEmpty() )
				return;
			const char* pTop = data.Top();
			const char* pEnd = data.End();
			const char* p    = nullptr;
			switch( *pTop ) {
			case 0x22:	/* double-quote */
			case 0x27:	/* single-quote */
				p = std::find( pTop + 1, pEnd, *pTop );
				if( p < pEnd ) {
					callback( TextSpan{ pTop + 1, p }, pOpaque );
					data.Chomp( p - pTop + 1, 0 );
				} else {
			default:
					callback( data.CutNextToken(), pOpaque );
				}
				break;
			}
		}
	}

	std::ostream& operator<<( std::ostream& os, const TextSpan& txt ) {
		os.write( txt.Top(), txt.ByteLength() );
		return os;
	}
	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	/*
	 * `xxx` で <code>xxx</code>
	 * アスタリスクもしくはアンダースコア3個で文字列を囲むことで <em> と <strong> 強調
	 * アスタリスクもしくはアンダースコア2個で文字列を囲むことで <strong> 強調
	 * アスタリスクもしくはアンダースコア1個で文字列を囲むことで <em> 強調
	 * チルダ2個で文字列を囲むことで取消線
	 * [表示文字](URL) 形式でリンク
	 * [表示文字](#見出し文字列) 形式で見出しへのページ内リンク
	 */
	static void WriteTextSpanImp( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink ) {
		const char* starts[count_of(s_operators)];
		const char*   ends[count_of(s_operators)];
		std::fill( starts, starts + count_of(s_operators), nullptr );

		while( pTop < pEnd ) {
			// 各装飾類の次の出現位置を必要に応じて検索（みつからない場合は pEnd になる）
			uint32_t top = count_of(s_operators);
			const char* pStart = pEnd;
			for( uint32_t i = 0; i < count_of(s_operators); ++i ) {
				if( starts[i] < pTop )
					starts[i] = s_operators[i].finder( pTop, pEnd, ends[i], whole );
				if( starts[i] < pStart ) {
					top  = i;
					pStart = starts[i];
				}
			}
			// どれもみつからない場合は最後まで出力してループ脱出
			if( pStart == pEnd ) {
				WriteWithTermLink( os, pTop, pEnd, docInfo, bTermLink );
				pTop = pEnd;
				break;
			}
			//最初の装飾の開始位置まで普通に出力
			WriteWithTermLink( os, pTop, starts[top], docInfo, bTermLink );
			//最初の装飾のオペレータ関数を実行して出力
			s_operators[top].writer( os, whole, starts[top], ends[top], docInfo, bTermLink );
			pTop = ends[top];
		}
	}


	// ^xxx^ 形式（sup）を処理する
	static const char* FindSupPattern( const char* pTop, const char* pEnd, 
									   const char*& endPos, const TextSpan& whole) {
		(void)whole;
		// ^...^ のパターンを検索する
		auto p1 = std::find( pTop, pEnd, '^' );
		if( p1 == pEnd )
			return pEnd;
		auto p2 = std::find( p1 + 2, pEnd, '^' );
		if( p2 == pEnd )
			return pEnd;
		endPos = p2 + 1;
		return p1;
	}
	static void OperateSup( std::ostream& os, const TextSpan& whole,
							const char* pTop, const char* pEnd,
							DocumentInfo& docInfo, bool bTermLink ) {
		assert( pTop[0]  == '^' );
		assert( pEnd[-1] == '^' );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "sup" );
		WriteTextSpanImp( os, whole, pTop + 1, pEnd - 1, docInfo, bTermLink );	// タグ内部でさらに他を適用
		os << "</sup>";
	}

	// ~xxx~ 形式（sub）を処理する
	static const char* FindSubPattern( const char* pTop, const char* pEnd, 
									   const char*& endPos, const TextSpan& whole) {
		(void)whole;
		// ~...~ のパターンを検索する（ただし ~~ はダメ）
		auto p1 = std::find( pTop, pEnd, '~' );
		if( p1 == pEnd || p1[1] == '~' )
			return pEnd;
		auto p2 = std::find( p1 + 2, pEnd, '~' );
		if( p2 == pEnd || p2[1] == '~' )
			return pEnd;
		endPos = p2 + 1;
		return p1;
	}
	static void OperateSub( std::ostream& os, const TextSpan& whole,
							const char* pTop, const char* pEnd,
							DocumentInfo& docInfo, bool bTermLink ) {
		assert( pTop[0]  == '~' );
		assert( pEnd[-1] == '~' );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "sub" );
		WriteTextSpanImp( os, whole, pTop + 1, pEnd - 1, docInfo, bTermLink );	// タグ内部でさらに他を適用
		os << "</sub>";
	}

	// ==xxx== 形式（mark）を処理する
	static const char* FindMarkPattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "==", 2, pTop, pEnd, endPos,  whole );
	}
	static void OperateMark( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink ) {
		assert( pTop[0] == '=' && pTop[1] == '=' );
		assert( pEnd[-1] == '=' && pEnd[-2] == '=' );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "mark" );
		WriteTextSpanImp( os, whole, pTop + 2, pEnd - 2, docInfo, bTermLink );	// 内部でさらに他を適用
		os << "</mark>";
	}

	// @{{STYLES}{CONTENTS}} / @((STYLES)(CONTENTS)) 形式を処理する
	static const char* FindStylesPatternImpl( const char* pPattern,
											  const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole ) {
		(void)whole;
		const char* p1 = std::search( pTop, pEnd, pPattern, pPattern + 3 );
		if( p1 == pEnd )
			return pEnd;
		const char* p2 = std::search( p1 + 3, pEnd, pPattern + 3, pPattern + 5 );
		if( p2 == pEnd )
			return pEnd;
		const char* p3 = std::search( p2 + 2, pEnd, pPattern + 5, pPattern + 7 );
		if( p3 == pEnd )
			return pEnd;
		endPos = p3 + 2;
		return p1;
	}
	static const char* FindStylesPattern1( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole ) {
		return FindStylesPatternImpl( "@{{}{}}", pTop, pEnd, endPos, whole );
	}
	static const char* FindStylesPattern2( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole ) {
		return FindStylesPatternImpl( "@(()())", pTop, pEnd, endPos, whole );
	}
	static void OperateStyles( std::ostream& os, const TextSpan& whole,
							   const char* pTop, const char* pEnd,
							   DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)docInfo;
		(void)bTermLink;
		assert( pTop[0] == '@' && (pTop[1] == '{' || pTop[1] == '(') && pTop[1] == pTop[2] );
		assert( (pEnd[-1] == '}' || pEnd[-1] == ')') && pEnd[-1] == pEnd[-2] );
		char target[2] = { pEnd[-1], pTop[1] };
		const char* delim = std::search( pTop + 3, pEnd, target, target + 2 );
		assert( delim < pEnd );
		os << "<span style='";	//MEMO : ignore StyleStack.
		os.write( pTop + 3, delim - (pTop + 3) );
		os << "'>";
		os.write( delim + 2, (pEnd - 2) - (delim + 2) );
		os << "</span>";
	}

	// [label](URL) 形式のリンクを処理する
	static const char* FindLinkPattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole) {
		(void)whole;
		static const char* const PATTERN = "[]()";
		const char* p1 = std::find( pTop, pEnd, PATTERN[0] );
		if( p1 == pEnd )
			return pEnd;
		const char* p2 = std::search( p1 + 1, pEnd, PATTERN + 1, PATTERN + 3 );
		if( p2 == pEnd )
			return pEnd;
		const char* p3 = std::find( p2 + 2, pEnd, PATTERN[3] );
		if( p3 == pEnd )
			return pEnd;
		endPos = p3 + 1;
		return p1;
	}
	static void OperateLink( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		const char* target = "](";
		auto p1 = std::search( pTop + 1, pEnd, target, target + 2 );
		assert( pTop[0] == '[' );
		assert( pEnd[-1] == ')' );
		assert( p1 < pEnd );
		auto pLbl1 = pTop + 1;
		auto pLbl2 = p1;
		auto pURL1 = p1 + 2;
		auto pURL2 = pEnd - 1;
		// URL 部分が空の場合そのまま出力して終了
		if( !(pURL2 - pURL1) ) {
			WriteWithTermLink( os, pTop, pEnd+1, docInfo, bTermLink );
			return;
		}
		// ページ内アンカー指定の場合
		ToC::EntryT tocType;
		if( ToC::IsInternalLink( pURL1, tocType ) == true ) {
			if( tocType == ToC::EntryT::HEADER )
				OperateInnerLink( os, tocType, pLbl1, pLbl2, pURL1 + 1, pURL2, docInfo );
			else
				OperateInnerLink( os, tocType, pLbl1, pLbl2, pURL1 + 2, pURL2, docInfo );
		// ページ内アンカー指定でない場合
		} else {
			os << "<a href='";
			os.write( pURL1, pURL2 - pURL1 );
			os << "'>";
			if( pLbl1 == pLbl2 )
				TextSpan::WriteWithEscape( os, pURL1, pURL2 );
			else {
				TextSpan tmp{ pLbl1, pLbl2 };
				tmp.WriteTo( os, docInfo, false );
			}
			os << "</a>";
		}
	}

	static void OperateInnerLink( std::ostream& os, ToC::EntryT type,
								  const char* pLbl1, const char* pLbl2,
								  const char* pURL1, const char* pURL2, DocumentInfo& docInfo ) {
		bool bEmptyLabel = (pLbl1 == pLbl2);
		auto& toc = docInfo.Get<ToC>();
		const char* pAnchor = toc.GetAnchorTag( type, pURL1, pURL2 );
		if( !pAnchor ) {
			std::cerr << "ERROR : invalid link anchor '";
			std::cerr.write( pURL1, pURL2 - pURL1 );
			std::cerr << "'." << std::endl;
			os << "<font color='red'>!!!ERROR : invalid internal anchor '";	//MEMO : ignore StyleStack.
			os.write( pURL1, pURL2 - pURL1 );
			os << "'.</font>";
			return;
		}
		os << "<a href='#" << pAnchor << "'>";
		auto& cfg = docInfo.Get<Config>();
		TextSpan entry{ pURL1, pURL2 };
		char prefix[64];
		// label 部分が空の場合
		if( bEmptyLabel ) {
			// header numbering または TABLE/FIGURE の場合は entry number 出力
			if( cfg.bNumberingHeader || type == ToC::EntryT::TABLE || type == ToC::EntryT::FIGURE ) {
				if( toc.GetEntryNumber( prefix, type, cfg, pURL1, pURL2 ) ) {
					os << prefix;
					if( !!*prefix )
						os << ' ';
				}
			}
			// url で指定された部分を出力
			entry.WriteTo( os, docInfo, false );

		// label 部分が空でない場合
		} else {
			while( pLbl1 < pLbl2 ) {
				const char* pPivot = FindLinkLabelPlaceHolder( pLbl1, pLbl2 );
				if( pLbl1 < pPivot )
					TextSpan{ pLbl1, pPivot }.WriteTo( os, docInfo, false );
				pLbl1 = pPivot;
				if( pLbl1 < pLbl2 ) {
					switch( pLbl1[1] ) {
					case '$':
						entry.WriteTo( os, docInfo, false );
						break;
					case '@':
						if( toc.GetEntryNumber( prefix, type, cfg, pURL1, pURL2 ) ) {
							if( !!*prefix )
								os << prefix;
							else
								std::cerr << "WARNING : entry number for '"
														  << entry << "' is void." << std::endl;
						}
						break;
					}
					pLbl1 += 2;
				}
			}
		}
		os << "</a>";
	}

	static const char* FindLinkLabelPlaceHolder( const char* pLabelTop,
												 const char* pLabelEnd ) {
		while( pLabelTop < pLabelEnd ) {
			const char* p = std::find( pLabelTop, pLabelEnd, '$' );
			if( p == pLabelEnd )
				return pLabelEnd;
			switch( p[1] ) {
			case '$':	return p;
			case '@':	return p;
			default:	pLabelTop = p + 1;
			}
		}
		return pLabelEnd;
	}

	// <br> 形式の（一部の許可された）HTMLタグを処理する
	static const char* FindTagsPattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole) {
		(void)whole;
		static const char* const PATTERN = "<br>";
		const char* p1 = std::search( pTop, pEnd, PATTERN, PATTERN + 4 );
		if( p1 == pEnd )
			return pEnd;
		endPos = p1 + 4;
		return p1;
	}
	static void OperateTags( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)pTop;
		(void)pEnd;
		(void)docInfo;
		(void)bTermLink;
		//MEMO : ひとまず <br> だけが対象なので、簡易的な実装にしておく
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "br" );
	}

	// `code` 形式を処理する
	static const char* FindCodePattern( const char* pTop, const char* pEnd, 
										const char*& endPos, const TextSpan& whole) {
		return FindSpaceRequiredPatternImpl( "`", 1, pTop, pEnd, endPos,  whole );
	}
	static void OperateCode( std::ostream& os, const TextSpan& whole,
							 const char* pTop, const char* pEnd,
							 DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)bTermLink;
		assert( pTop[0] == '`' );
		assert( pEnd[-1] == '`' );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "code" );
		// code タグ内部ではネストさせないし用語の自動リンクもしない
		TextSpan::WriteWithEscape( os, pTop + 1, pEnd - 1 );
		os << "</code>";
	}

	// $formula$ 形式を処理する
	static const char* FindMathJaxPattern( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "$", 1, pTop, pEnd, endPos,  whole );
	}
	static void OperateMathJax( std::ostream& os, const TextSpan& whole,
								const char* pTop, const char* pEnd,
								DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)docInfo;
		(void)bTermLink;
		assert( pTop[0] == '$' );
		assert( pEnd[-1] == '$' );
		os << "\\(";
		// 内部ではネストさせないし用語の自動リンクもしない
		TextSpan::WriteWithEscape( os, pTop + 1, pEnd - 1 );
		os << "\\)";
	}

	// ![alt](url) 形式を処理する
	static const char* FindImagePattern( const char* pTop, const char* pEnd, 
										 const char*& endPos, const TextSpan& whole) {
		(void)whole;
		static const char* const PATTERN = "![]()";
		const char* p1 = std::search( pTop, pEnd, PATTERN, PATTERN + 2 );
		if( p1 == pEnd )
			return pEnd;
		const char* p2 = std::search( p1 + 2, pEnd, PATTERN + 2, PATTERN + 4 );
		if( p2 == pEnd )
			return pEnd;
		const char* p3 = std::find( p2 + 2, pEnd, PATTERN[4] );
		if( p3 == pEnd )
			return pEnd;
		endPos = p3 + 1;
		return p1;
	}
	static void OperateImage( std::ostream& os, const TextSpan& whole,
							  const char* pTop, const char* pEnd,
							  DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)bTermLink;
		TextSpan tmp{ pTop, pEnd };
		TextSpan alt;
		TextSpan url;
		if( tmp.IsMatch( "![", alt, "](", url, ")" ) == false ) {
			assert( false );
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "img", nullptr, " src='" );
		os.write( url.Top(), url.ByteLength() );
		os << "' ";
		if( alt.IsEmpty() == false ) {
			os << "alt='";
			alt.WriteTo( os, docInfo );
			os << "' ";
		}
		os << "/>";
	}

	// ~~string~~ 形式を処理する
	static const char* FindStrikePattern( const char* pTop, const char* pEnd, 
										  const char*& endPos, const TextSpan& whole) {
		return FindSpaceRequiredPatternImpl( "~~", 2, pTop, pEnd, endPos,  whole );
	}
	static void OperateStrike( std::ostream& os, const TextSpan& whole,
							   const char* pTop, const char* pEnd,
							   DocumentInfo& docInfo, bool bTermLink ) {
		assert( pTop[0] == '~' && pTop[1] == '~' );
		assert( pEnd[-1] == '~' && pEnd[-2] == '~' );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "strike" );
		WriteTextSpanImp( os, whole, pTop + 2, pEnd - 2, docInfo, bTermLink );	// 内部でさらに他を適用
		os << "</strike>";
	}

	// ___string___, ***string*** 　形式による強調を処理する
	static const char* FindEmphasis3Pattern1( const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "___", 3, pTop, pEnd, endPos,  whole );
	}
	static const char* FindEmphasis3Pattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "***", 3, pTop, pEnd, endPos,  whole );
	}
	static void OperateEmphasis3( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink ) {
		assert( (pTop[0] == '_' || pTop[0] == '*') && pTop[0] == pTop[1] && pTop[1] == pTop[2] );
		assert( pEnd[-1] == pTop[0] && pEnd[-2] == pEnd[-1] && pEnd[-3] == pEnd[-2] );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "em" );
		styles.WriteOpenTag( os, "strong" );
		WriteTextSpanImp( os, whole, pTop + 3, pEnd - 3, docInfo, bTermLink );	// 内部でさらに他を適用
		os << "</strong></em>";
	}

	// __string__, **string** 　形式による強調を処理する
	static const char* FindEmphasis2Pattern1( const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "__", 2, pTop, pEnd, endPos,  whole );
	}
	static const char* FindEmphasis2Pattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "**", 2, pTop, pEnd, endPos,  whole );
	}
	static void OperateEmphasis2( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink ) {
		assert( pTop[0] == '_' || pTop[0] == '*' );
		assert( pTop[0] == pTop[1] );
		assert( pEnd[-1] == pTop[0] );
		assert( pEnd[-2] == pEnd[-1] );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "strong" );
		WriteTextSpanImp( os, whole, pTop + 2, pEnd - 2, docInfo, bTermLink );	// 内部でさらに他を適用
		os << "</strong>";
	}

	// _string_, *string* 　形式による強調を処理する
	static const char* FindEmphasis1Pattern1( const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "_", 1, pTop, pEnd, endPos,  whole );
	}
	static const char* FindEmphasis1Pattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole ) {
		return FindSpaceRequiredPatternImpl( "*", 1, pTop, pEnd, endPos,  whole );
	}
	static void OperateEmphasis1( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink ) {
		assert( (pTop[0] == '_' || pTop[0] == '*') );
		assert( pEnd[-1] == pTop[0] );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "em" );
		WriteTextSpanImp( os, whole, pTop + 1, pEnd - 1, docInfo, bTermLink );	// 内部でさらに他を適用
		os << "</em>";
	}

	// {{fn:string}} 形式の脚注を処理する
	static const char* FindFootnotePattern( const char* pTop, const char* pEnd, 
											const char*& endPos, const TextSpan& whole) {
		(void)whole;
		static const char* const PATTERN = "{{fn:}}";
		const char* p1 = std::search( pTop, pEnd, PATTERN, PATTERN + 5 );
		if( p1 == pEnd )
			return pEnd;
		const char* p2 = std::search( p1, pEnd, PATTERN + 5, PATTERN + 7 );
		if( p2 == pEnd )
			return pEnd;
		endPos = p2 + 2;
		return p1;
	}
	static void OperateFootnote( std::ostream& os, const TextSpan& whole,
								 const char* pTop, const char* pEnd,
								 DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)bTermLink;
		uint32_t idx = docInfo.Get<Footnotes>().Register( pTop + 5, pEnd - 2 );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "sup" )
		   << "<a name='footnote_ref" << idx << "' "
		   << "href='#footnote" << idx << "'>"
		   << idx
		   << "</a></sup>";
	}

	// {{fn(TAG):string}} 形式の脚注を処理する
	static const char* FindFootnotePattern2( const char* pTop, const char* pEnd, 
											 const char*& endPos, const TextSpan& whole ) {
		(void)whole;
		static const char* const PATTERN = "{{fn():}}";
		const char* p1 = std::search( pTop, pEnd, PATTERN, PATTERN + 5 );
		if( p1 == pEnd )
			return pEnd;
		const char* p2 = std::search( p1, pEnd, PATTERN + 5, PATTERN + 7 );
		if( p2 == pEnd )
			return pEnd;
		const char* p3 = std::search( p1, pEnd, PATTERN + 7, PATTERN + 9 );
		if( p3 == pEnd )
			return pEnd;
		endPos = p3 + 2;
		return p1;
	}
	static void OperateFootnote2( std::ostream& os, const TextSpan& whole,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)bTermLink;
		TextSpan tmp{ pTop, pEnd };
		TextSpan tag;
		TextSpan note;
		if( tmp.IsMatch( "{{fn(", tag, "):", note, "}}" ) ) {
			uint32_t idx = docInfo.Get<Footnotes>().Register( tag, note );
			auto& styles = docInfo.Get<StyleStack>();
			styles.WriteOpenTag( os, "sup" )
			   << "<a name='footnote_ref_" << tag << "_" << idx << "' "
			   << "href='#footnote_" << tag << "_" << idx << "'>"
			   << idx
			   << "</a></sup>";
		}
	}

	// #((ANCHOR)) 形式を処理する
	static const char* FindAnchorPatternImpl( const char* pPattern,
											  const char* pTop, const char* pEnd, 
											  const char*& endPos, const TextSpan& whole ) {
		(void)whole;
		const char* p1 = std::search( pTop, pEnd, pPattern, pPattern + 3 );
		if( p1 == pEnd )
			return pEnd;
		const char* p2 = std::search( p1 + 3, pEnd, pPattern + 3, pPattern + 5 );
		if( p2 == pEnd )
			return pEnd;
		endPos = p2 + 2;
		return p1;
	}
	static const char* FindAnchorPattern1( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole ) {
		return FindAnchorPatternImpl( "#(())", pTop, pEnd, endPos, whole );
	}
	static const char* FindAnchorPattern2( const char* pTop, const char* pEnd, 
										   const char*& endPos, const TextSpan& whole ) {
		return FindAnchorPatternImpl( "#{{}}", pTop, pEnd, endPos, whole );
	}
	static void OperateAnchor( std::ostream& os, const TextSpan& whole,
							   const char* pTop, const char* pEnd,
							   DocumentInfo& docInfo, bool bTermLink ) {
		(void)whole;
		(void)docInfo;
		(void)bTermLink;
		assert( pTop[0] == '#' && (pTop[1] == '{' || pTop[1] == '(') && pTop[1] == pTop[2] );
		assert( (pEnd[-1] == '}' || pEnd[-1] == ')') && pEnd[-1] == pEnd[-2] );
		auto& toc    = docInfo.Get<ToC>();
		const char* pTag = toc.GetAnchorTag( ToC::EntryT::ANCHOR, pTop + 3, pEnd - 2 );
		if( pTag )
			os << "<a name='" << pTag << "'></a>";
		else {
			//ToDo : error message...
		}
	}

	static const char* FindSpaceRequiredPatternImpl( const char* pPattern, uint32_t len,
													 const char* pTop, const char* pEnd, 
													 const char*& endPos, const TextSpan& whole ) {
		const char* p1 = pTop;
		while( p1 < pEnd ) {
			p1 = std::search( p1, pEnd, pPattern, pPattern + len );
			if( p1 == pEnd )
				return pEnd;
			if( IsTopOfSpaceRequiredOperator( p1, p1 + len, whole ) )
				break;
			p1 += len;
		}
		if( pEnd <= p1 )
			return pEnd;
		const char* p2 = p1 + (len+1);	// (len+1) means 'need one more character'.
		while( p2 < pEnd ) {
			p2 = std::search( p2, pEnd, pPattern, pPattern + len );
			if( p2 == pEnd )
				return pEnd;
			if( IsEndOfSpaceRequiredOperator( p2, p2 + len, whole ) )
				break;
			p2 += len;
		}
		if( pEnd <= p2 )
			return pEnd;
		endPos = p2 + len;
		return p1;
	}

	static void WriteWithTermLink( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink ) {
		if( !bTermLink )
			TextSpan::WriteWithEscape( os, pTop, pEnd );
		else {
			auto& glossary = docInfo.Get<Glossary>();
			glossary.WriteWithTermLink( os, pTop, pEnd, TextSpan::WriteWithEscape );
		}
	}

} // namespace turnup
