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

#include <string.h>
#include <iostream>
#include <algorithm>

namespace turnup {

	//MEMO : variadic template が使えない環境も考慮に入れて...
	template <typename T>
	inline T min3( T t1, T t2, T t3 ) {
		return std::min( t1, std::min( t2, t3 ) );
	}
	template <typename T>
	inline T min11( T t0, T t1, T t2, T t3, T t4, T t5, T t6, T t7, T t8, T t9, T t10 ) {
		return min3( t0, t1, min3( min3( t2, t3, t4 ),
								   min3( t5, t6, t7 ),
								   min3( t8, t9, t10 ) ) );
	}

	static void WriteTextSpanImp( std::ostream& os,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink );
	// ^xxx^ 形式（sup）を処理する
	static const char* OperateSup( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink );
	// ~xxx~ 形式（sub）を処理する
	static const char* OperateSub( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink );
	// ==xxx== 形式（mark）を処理する
	static const char* OperateMark( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink );
	// @{{styles}{xxxxxx}} 形式（styles）を処理する
	static const char* OperateStyles( std::ostream& os,
									  const char* pTop, const char* pEnd,
									  DocumentInfo& docInfo, bool bTermLink );
	// [label](URL) 形式のリンクを処理する
	static const char* OperateLink( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink );
	static void OperateInnerLink( std::ostream& os, ToC::EntryT type,
								  const char* pLbl1, const char* pLbl2,
								  const char* pURL1, const char* pURL2,
								  DocumentInfo& docInfo );
	static const char* FindLinkLabelPlaceHolder( const char* pLabelTop,
												 const char* pLabelEnd );
	// <br> 形式の（一部の許可された）HTMLタグを処理する
	static const char* OperateTags( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink );
	// `code` 形式を処理する
	static const char* OperateCode( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink );
	// ![alt](url) 形式を処理する
	static const char* OperateImage( std::ostream& os,
									 const char* pTop, const char* pEnd,
									 DocumentInfo& docInfo, bool bTermLink );
	// ~~string~~ 形式を処理する
	static const char* OperateStrike( std::ostream& os,
									  const char* pTop, const char* pEnd,
									  DocumentInfo& docInfo, bool bTermLink );
	// *string*, **string**, ***string***,
	// _string_, __string__, ___string___ 　の各種形式による強調を処理する
	static const char* OperateEmphasis( std::ostream& os,
										const char* pTop, const char* pEnd,
										DocumentInfo& docInfo, bool bTermLink );
	// {{fn:string}} 形式の脚注を処理する
	static const char* OperateFootnote( std::ostream& os,
										const char* pTop, const char* pEnd,
										DocumentInfo& docInfo, bool bTermLink );
	static void WriteWithTermLink( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink );
	static void WriteWithEscape( std::ostream& os,
								 const char* pTop, const char* pEnd );

	//--------------------------------------------------------------------------
	//
	// implementation of class TextSpan
	//
	//--------------------------------------------------------------------------
	TextSpan::TextSpan() : m_pTop( nullptr ), m_pEnd( nullptr ) {
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
		WriteTextSpanImp( os, this->Top(), this->End(), docInfo, bTermLink );
		return os;
	}

	std::ostream& TextSpan::WriteSimple( std::ostream& os ) const {
		WriteWithEscape( os, this->Top(), this->End() );
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
	static void WriteTextSpanImp( std::ostream& os,
								  const char* pTop, const char* pEnd,
								  DocumentInfo& docInfo, bool bTermLink ) {
		auto i00 = std::find( pTop, pEnd, '!' );
		auto i01 = std::find( pTop, pEnd, '*' );
		auto i02 = std::find( pTop, pEnd, '<' );
		auto i03 = std::find( pTop, pEnd, '=' );
		auto i04 = std::find( pTop, pEnd, '@' );
		auto i05 = std::find( pTop, pEnd, '[' );
		auto i06 = std::find( pTop, pEnd, '^' );
		auto i07 = std::find( pTop, pEnd, '_' );
		auto i08 = std::find( pTop, pEnd, '`' );
		auto i09 = std::find( pTop, pEnd, '{' );
		auto i10 = std::find( pTop, pEnd, '~' );

		while( pTop < pEnd ) {
			auto pStart = min11( i00, i01, i02, i03, i04, i05, i06, i07, i08, i09, i10 );
			if( pStart == pEnd ) {
				WriteWithTermLink( os, pTop, pEnd, docInfo, bTermLink );
				pTop = pEnd;
				break;
			}
			//発見された開始位置までは普通に出力
			WriteWithTermLink( os, pTop, pStart, docInfo, bTermLink );
			pTop = pStart;
			//発見された要素で分岐
			switch( *pTop ) {
			case '!':	pTop = OperateImage(    os, pTop, pEnd, docInfo, bTermLink );	break;
			case '*':	pTop = OperateEmphasis( os, pTop, pEnd, docInfo, bTermLink );	break;
			case '<':	pTop = OperateTags(     os, pTop, pEnd, docInfo, bTermLink );	break;
			case '=':	pTop = OperateMark(     os, pTop, pEnd, docInfo, bTermLink );	break;
			case '@':	pTop = OperateStyles(   os, pTop, pEnd, docInfo, bTermLink );	break;
			case '[':	pTop = OperateLink(     os, pTop, pEnd, docInfo, bTermLink );	break;
			case '^':	pTop = OperateSup(      os, pTop, pEnd, docInfo, bTermLink );	break;
			case '_':	pTop = OperateEmphasis( os, pTop, pEnd, docInfo, bTermLink );	break;
			case '`':	pTop = OperateCode(     os, pTop, pEnd, docInfo, bTermLink );	break;
			case '{':	pTop = OperateFootnote( os, pTop, pEnd, docInfo, bTermLink );	break;
			case '~':
					if( pTop[1] != '~' )
						pTop = OperateSub(      os, pTop, pEnd, docInfo, bTermLink );
					else
						pTop = OperateStrike(   os, pTop, pEnd, docInfo, bTermLink );
					break;
			}
			i00 = std::find( pTop, pEnd, '!' );
			i01 = std::find( pTop, pEnd, '*' );
			i02 = std::find( pTop, pEnd, '<' );
			i03 = std::find( pTop, pEnd, '=' );
			i04 = std::find( pTop, pEnd, '@' );
			i05 = std::find( pTop, pEnd, '[' );
			i06 = std::find( pTop, pEnd, '^' );
			i07 = std::find( pTop, pEnd, '_' );
			i08 = std::find( pTop, pEnd, '`' );
			i09 = std::find( pTop, pEnd, '{' );
			i10 = std::find( pTop, pEnd, '~' );
		}
	}


	// ^xxx^ 形式（sup）を処理する
	static const char* OperateSup( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink ) {
		auto p = std::find( pTop + 1, pEnd, '^' );
		if( p == pEnd ) {
			os << "^";
			return pTop + 1;
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "sup" );
		WriteTextSpanImp( os, pTop + 1, p, docInfo, bTermLink );	// sup タグ内部でさらに他を適用する
		os << "</sup>";
		return p + 1;
	}

	// ~xxx~ 形式（sub）を処理する
	static const char* OperateSub( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink ) {
		auto p = std::find( pTop + 1, pEnd, '~' );
		if( p == pEnd ) {
			os << "~";
			return pTop + 1;
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "sub" );
		WriteTextSpanImp( os, pTop + 1, p, docInfo, bTermLink );	// sub タグ内部でさらに他を適用する
		os << "</sub>";
		return p + 1;
	}

	// ==xxx== 形式（mark）を処理する
	static const char* OperateMark( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink ) {
		const char* target = "==";
		// = が２連続でなければ無視
		if( !!::strncmp( pTop, target, 2 ) ) {
			os.write( pTop, 1 );
			return pTop + 1;
		}
		//終端を検索
		auto p = std::search( pTop + 2, pEnd, target, target + 2 );
		if( p == pEnd ) {
			os << "==";
			return pTop + 2;
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "mark" );
		WriteTextSpanImp( os, pTop + 2, p, docInfo, bTermLink );	// mark タグ内部でさらに他を適用する
		os << "</mark>";
		return p + 2;
	}

	// @{{styles}{xxxxxx}} 形式（styles）を処理する
	static const char* OperateStyles( std::ostream& os,
									  const char* pTop, const char* pEnd,
									  DocumentInfo& docInfo, bool bTermLink ) {
		(void)docInfo;
		(void)bTermLink;
		// @{{ で始まっていなければ無視
		if( !!::strncmp( pTop, "@{{", 3 ) ) {
			os.write( pTop, '@' ); 
			return pTop + 1;
		}
		const char* target = "}{";
		const char* end1 = std::search( pTop + 3, pEnd, target, target + 2 );
		target = "}}";
		const char* end2 = std::search( pTop + 3, pEnd, target, target + 2 );
		if( end1 == pEnd || end2 == pEnd || end2 < end1 ) {
			os.write( pTop, '@' ); 
			return pTop + 1;
		}
		os << "<span style='";	//MEMO : ignore StyleStack.
		os.write( pTop + 3, end1 - (pTop + 3) );
		os << "'>";
		os.write( end1 + 2, end2 - (end1 + 2) );
		os << "</span>";
		return end2 + 2;
	}

	// [label](URL) 形式のリンクを処理する
	static const char* OperateLink( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink ) {
		(void)bTermLink;
		const char* target = "](";
		//終端1を検索
		auto p1 = std::search( pTop + 1, pEnd, target, target + 2 );
		if( p1 == pEnd ) {
			os << "[";
			return pTop + 1;
		}
		//終端2を検索
		auto p2 = std::find( p1 + 2, pEnd, ')' );
		if( p2 == pEnd ) {
			os << "[";
			return pTop + 1;
		}
		auto pLbl1 = pTop + 1;
		auto pLbl2 = p1;
		auto pURL1 = p1 + 2;
		auto pURL2 = p2;
		// URL 部分が空の場合そのまま出力して終了
		if( !(pURL2 - pURL1) ) {
			WriteWithTermLink( os, pTop, (p2+1), docInfo, bTermLink );
			return p2 + 1;
		}
		// ページ内アンカー指定の場合
		if( pURL1[0] == '#' )
			OperateInnerLink( os, ToC::EntryT::HEADER,
							  pLbl1, pLbl2, pURL1 + 1, pURL2, docInfo );
		else if( (pURL1[0] == 'T' || pURL1[0] == 'F') && pURL1[1] == '#' )
			OperateInnerLink( os,
							  pURL1[0] == 'T' ? ToC::EntryT::TABLE
											  : ToC::EntryT::FIGURE,
							  pLbl1, pLbl2, pURL1 + 2, pURL2, docInfo );
		// ページ内アンカー指定でない場合
		else {
			if( pLbl1 == pLbl2 ) {
				pLbl1 = pURL1;
				pLbl2 = pURL2;
			}
			os << "<a href='";
			os.write( pURL1, pURL2 - pURL1 );
			os << "'>";
			WriteWithEscape( os, pLbl1, pLbl2 );
			os << "</a>";

		}
		return p2 + 1;
	}

	static void OperateInnerLink( std::ostream& os, ToC::EntryT type,
								  const char* pLbl1, const char* pLbl2,
								  const char* pURL1, const char* pURL2,
								  DocumentInfo& docInfo ) {
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
		char prefix[64];
		if( bEmptyLabel ) {
			if( cfg.bNumberingHeader || type != ToC::EntryT::HEADER ) {
				if( toc.GetEntryNumber( prefix, type, cfg, pURL1, pURL2 ) )
					os << prefix << ' ';
			}
			WriteWithEscape( os, pURL1, pURL2 );
		} else {
			while( pLbl1 < pLbl2 ) {
				const char* pPivot = FindLinkLabelPlaceHolder( pLbl1, pLbl2 );
				if( pLbl1 < pPivot )
					WriteWithEscape( os, pLbl1, pPivot );
				pLbl1 = pPivot;
				if( pLbl1 < pLbl2 ) {
					switch( pLbl1[1] ) {
					case '$':
						WriteWithEscape( os, pURL1, pURL2 );
						break;
					case '@':
						if( toc.GetEntryNumber( prefix, type, cfg, pURL1, pURL2 ) )
							os << prefix;
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
	static const char* OperateTags( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink ) {
		(void)pEnd;
		(void)docInfo;
		(void)bTermLink;
		//MEMO : ひとまず <br> だけが対象なので、簡易的な実装にしておく

		// <br> でなければ無視
		if( !!::strncmp( pTop, "<br>", 4 ) ) {
			os << "&lt;";
			return pTop + 1;
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "br" );
		return pTop + 4;
	}

	// `code` 形式を処理する
	static const char* OperateCode( std::ostream& os,
									const char* pTop, const char* pEnd,
									DocumentInfo& docInfo, bool bTermLink ) {
		(void)docInfo;
		(void)bTermLink;
		auto p = std::find( pTop + 1, pEnd, '`' );
		if( p == pEnd ) {
			os << "`";
			return pTop + 1;
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "code" );
		WriteWithEscape( os, pTop + 1, p );	// code タグ内部ではネストさせないし用語の自動リンクもしない
		os << "</code>";
		return p + 1;
	}

	// ![alt](url) 形式を処理する
	static const char* OperateImage( std::ostream& os,
									 const char* pTop, const char* pEnd,
									 DocumentInfo& docInfo, bool bTermLink ) {
		(void)bTermLink;
		// 終端の ) を検索
		auto pDelim = std::find( pTop, pEnd, ')' );
		// 終端が見つからない場合は ! だけ出力して終了
		if( pDelim == pEnd ) {
			os << "!";
			return pTop + 1;
		}
		TextSpan tmp{ pTop, pDelim+1 };
		TextSpan alt;
		TextSpan url;
		if( tmp.IsMatch( "![", alt, "](", url, ")" ) == false ) {
			os << "!";
			return pTop + 1;
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
		return pDelim + 1;
	}

	// ~~string~~ 形式を処理する
	static const char* OperateStrike( std::ostream& os,
									  const char* pTop, const char* pEnd,
									  DocumentInfo& docInfo, bool bTermLink ) {
		const char* target = "~~";
		//チルダが２連続でなければ無視
		if( !!::strncmp( pTop, target, 2 ) ) {
			os.write( pTop, 1 );
			return pTop + 1;
		}
		//終端を検索
		auto p = std::search( pTop + 2, pEnd, target, target + 2 );
		if( p == pEnd ) {
			os << "~~";
			return pTop + 2;
		}
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "strike" );
		WriteTextSpanImp( os, pTop + 2, p, docInfo, bTermLink );	// 取り消し線内部でさらに他を適用する
		os << "</strike>";
		return p + 2;
	}

	// *string*, **string**, ***string***,
	// _string_, __string__, ___string___ 　の各種形式による強調を処理する
	static const char* OperateEmphasis( std::ostream& os,
										const char* pTop, const char* pEnd,
										DocumentInfo& docInfo, bool bTermLink ) {
		//単独か、２連続か、３連続かを特定
		uint32_t cnt = 1;
		if( pTop[1] == pTop[0] ) {
			++cnt;
			if( pTop[2] == pTop[0] )
				++cnt;
		}
		//終端を検索
		//ToDo : 終端検索に失敗した場合、より短いのであれば成功するかも。どうやる？
		auto p = std::search( pTop + cnt, pEnd, pTop, pTop + cnt );
		if( p == pEnd ) {
			os.write( pTop, cnt );
			return pTop + cnt;
		}
		auto& styles = docInfo.Get<StyleStack>();
		if( cnt & 1 )	styles.WriteOpenTag( os, "em" );
		if( cnt & 2 )	styles.WriteOpenTag( os, "strong" );
		WriteTextSpanImp( os, pTop + cnt, p, docInfo, bTermLink );	// 強調内部でさらに他を適用する
		if( cnt & 2 )	os << "</strong>";
		if( cnt & 1 )	os << "</em>";
		return p + cnt;
	}

	// {{fn:string}} 形式の脚注を処理する
	static const char* OperateFootnote( std::ostream& os,
										const char* pTop, const char* pEnd,
										DocumentInfo& docInfo, bool bTermLink ) {
		(void)bTermLink;
		//無関係な open bracket だった場合は出力して終了
		if( !!::strncmp( pTop, "{{fn:", 5 ) ) {
			os << "{";
			return pTop + 1;
		}
		//終端を検索
		const char* target = "}}";
		auto p = std::search( pTop + 5, pEnd, target, target + 2 );
		if( p == pEnd ) {
			os.write( pTop, 5 );
			return pTop + 5;
		}
		uint32_t idx = docInfo.Get<Footnotes>().Register( pTop + 5, p );
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( os, "sup" )
		   << "<a name='footnote_ref" << idx << "' "
		   << "href='#footnote" << idx << "'>"
		   << idx
		   << "</a></sup>";
		return p + 2;
	}

	static void WriteWithTermLink( std::ostream& os,
								   const char* pTop, const char* pEnd,
								   DocumentInfo& docInfo, bool bTermLink ) {
		if( !bTermLink )
			WriteWithEscape( os, pTop, pEnd );
		else {
			auto& glossary = docInfo.Get<Glossary>();
			glossary.WriteWithTermLink( os, pTop, pEnd, WriteWithEscape );
		}
	}

	/*
	 * < を &lt; に
	 * > を &gt; に
	 * & を &amp; に
	 */
	static void WriteWithEscape( std::ostream& os,
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

} // namespace turnup
