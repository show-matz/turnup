//------------------------------------------------------------------------------
//
// InternalFilter4CommonLISP.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4CommonLISP.hxx"

#include "InternalFilter.hxx"
#include "TextSpan.hxx"
#include "FilterBuffer.hxx"
#include "Utilities.hxx"

#include <iostream>
#include <algorithm>
#include <string.h>

namespace turnup {

	static const char* SYMBOL_CHAR = "abcdefghijklmnopqrstuvwxyz"
									 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
									 "0123456789_<>-+*@?!$%&/[]^~{}.";

	static bool RangeCommentFinder( const TextSpan& span, const char* pTarget,
									TextSpan& result, const char*& className );
	static bool LineCommentFinder( const TextSpan& span, const char* pTarget,
								   TextSpan& result, const char*& className );
	static bool StringLiteralFinder( const TextSpan& span, const char* pTarget,
									 TextSpan& result, const char*& className );
	static bool DefinitionFinder( const TextSpan& span, const char* pTarget,
								  TextSpan& result, const char*& className );
	static bool GensymFinder( const TextSpan& span, const char* pTarget,
							  TextSpan& result, const char*& className );
	static bool KeywordSymbolFinder( const TextSpan& span, const char* pTarget,
									 TextSpan& result, const char*& className );
	static bool KeywordFinder( const TextSpan& span, const char* pTarget,
							   TextSpan& result, const char*& className );

	static const RangeFinderUnit s_units4CL[] = {
		{ RangeCommentFinder,	nullptr,    "comment"  },	//  #| ... |#
		{ LineCommentFinder,	nullptr,    "comment"  },	//  ;
		{ StringLiteralFinder,	nullptr,    "literal"  },	//  " ... "
		{ DefinitionFinder,		nullptr,    nullptr    },	//  (def... SYMBOL
		{ GensymFinder,			nullptr,    "keyword6" },	//  #:gensym
		{ KeywordSymbolFinder,	nullptr,    "keyword3" },	//  :keyword
		// language keyword
		{ KeywordFinder,		"block",                 "keyword1" },
		{ KeywordFinder,		"case",                  "keyword1" },
		{ KeywordFinder,		"catch",                 "keyword1" },
		{ KeywordFinder,		"cond",                  "keyword1" },
		{ KeywordFinder,		"declare",               "keyword1" },
		{ KeywordFinder,		"defclass",              "keyword1" },
		{ KeywordFinder,		"defconstant",           "keyword1" },
		{ KeywordFinder,		"defgeneric",            "keyword1" },
		{ KeywordFinder,		"define-compiler-macro", "keyword1" },
		{ KeywordFinder,		"define-condition",      "keyword1" },
		{ KeywordFinder,		"define-modify-macro",   "keyword1" },
		{ KeywordFinder,		"defmacro",              "keyword1" },
		{ KeywordFinder,		"defmethod",             "keyword1" },
		{ KeywordFinder,		"defpackage",            "keyword1" },
		{ KeywordFinder,		"defparameter",          "keyword1" },
		{ KeywordFinder,		"defsetf",               "keyword1" },
		{ KeywordFinder,		"defstruct",             "keyword1" },
		{ KeywordFinder,		"defun",                 "keyword1" },
		{ KeywordFinder,		"defvar",                "keyword1" },
		{ KeywordFinder,		"destructuring-bind",    "keyword1" },
		{ KeywordFinder,		"dolist",                "keyword1" },
		{ KeywordFinder,		"ecase",                 "keyword1" },
		{ KeywordFinder,		"error",                 "keyword1" },
		{ KeywordFinder,		"go",                    "keyword1" },
		{ KeywordFinder,		"handler-bind",          "keyword1" },
		{ KeywordFinder,		"handler-case",          "keyword1" },
		{ KeywordFinder,		"if",                    "keyword1" },
		{ KeywordFinder,		"in-package",            "keyword1" },
		{ KeywordFinder,		"labels",                "keyword1" },
		{ KeywordFinder,		"lambda",                "keyword1" },
		{ KeywordFinder,		"let",                   "keyword1" },
		{ KeywordFinder,		"let*",                  "keyword1" },
		{ KeywordFinder,		"locally",               "keyword1" },
		{ KeywordFinder,		"loop",                  "keyword1" },
		{ KeywordFinder,		"multiple-value-bind",   "keyword1" },
		{ KeywordFinder,		"proclaim",              "keyword1" },
		{ KeywordFinder,		"prog1",                 "keyword1" },
		{ KeywordFinder,		"progn",                 "keyword1" },
		{ KeywordFinder,		"restart-case",          "keyword1" },
		{ KeywordFinder,		"return",                "keyword1" },
		{ KeywordFinder,		"return-from",           "keyword1" },
		{ KeywordFinder,		"symbol-macrolet",       "keyword1" },
		{ KeywordFinder,		"the",                   "keyword1" },
		{ KeywordFinder,		"throw",                 "keyword1" },
		{ KeywordFinder,		"typecase",              "keyword1" },
		{ KeywordFinder,		"unless",                "keyword1" },
		{ KeywordFinder,		"unwind-protect",        "keyword1" },
		{ KeywordFinder,		"when",                  "keyword1" },
		{ KeywordFinder,		"with-gensyms",          "keyword1" },
		{ KeywordFinder,		"with-open-file",        "keyword1" },
		{ KeywordFinder,		"with-output-to-string", "keyword1" },
		{ KeywordFinder,		"with-slots",            "keyword1" },
		// &SYMBOL
		{ KeywordFinder,		"&rest",     "keyword2" },
		{ KeywordFinder,		"&optional", "keyword2" },
		{ KeywordFinder,		"&key",      "keyword2" },
		// endmark
		{ nullptr,				nullptr,    nullptr   }
	};

	// (def... に後続するシンボルを処理するためのテーブル
	static const struct DefEntry {
		const char* type;
		const char* clsName;
	} s_deftbl[] = {
		// variable definition
		{ "defparameter", "keyword4" },
		{ "defvar",       "keyword4" },
		{ "defconstant",  "keyword4" },
		// function/macro definition
		{ "defun",        "keyword5" },
		{ "defmacro",     "keyword5" },
		{ "defgeneric",   "keyword5" },
		{ "defmethod",    "keyword5" },
		{ "defsetf",      "keyword5" },
		// structure definition
		{ "defstruct",    "keyword2" },
		{ "defclass",     "keyword2" },
		{ "defpackage",   "keyword2" }
	};

	//------------------------------------------------------------------------------
	//
	// exported functions
	//
	//------------------------------------------------------------------------------
	bool InternalFilter4CommonLISP( std::ostream& os, DocumentInfo& docInfo,
									const TextSpan* pTop, const TextSpan* pEnd ) {
		(void)docInfo;
		FilterBuffer buf{ pTop, pEnd };
		TextSpan span = buf.GetBuffer();
		os << "<pre class='code'>" << std::endl;
		InternalFilter::ExecRecursive( os, span, s_units4CL + 0 );
		os << "</pre>" << std::endl;
		return true;

	}

	//------------------------------------------------------------------------------
	//
	// internal functions
	//
	//------------------------------------------------------------------------------
	static bool RangeCommentFinder( const TextSpan& span, const char* pTarget,
									TextSpan& result, const char*& className ) {
		(void)className;
		//MEMO : #| ... |# 形式のコメント
		//MEMO : 文字列中の |# とかを回避するのは現状では諦めてる．．．
		pTarget = "#|";
		auto p1 = std::search( span.Top(), span.End(), pTarget, pTarget + 2 );
		if( p1 == span.End() )
			return false;
		pTarget = "|#";
		auto p2 = std::search( p1+2, span.End(), pTarget, pTarget + 2 );
		if( p2 == span.End() )
			return false;
		result.Top() = p1;
		result.End() = p2 + 2;
		return true;
	}

	static bool LineCommentFinder( const TextSpan& span, const char* pTarget,
								   TextSpan& result, const char*& className ) {
		(void)pTarget;
		(void)className;
		//MEMO : ; ... 形式のコメント
		auto p1 = std::find( span.Top(), span.End(), ';' );
		if( p1 == span.End() )
			return false;
		auto p2 = std::find( p1, span.End(), 0x0A ); 
		result.Top() = p1;
		result.End() = p2;
		return true;
	}

	static bool StringLiteralFinder( const TextSpan& span, const char* pTarget,
									 TextSpan& result, const char*& className ) {
		(void)pTarget;
		(void)className;
		//エスケープされていない double quote を検索する
		const char* p1 = span.Top();
		while( true ) {
			p1 = std::find( p1, span.End(), '"' ); 
			if( p1 == span.End() )
				return false;
			if( p1[-1] != '\\' )
				break;
			++p1;
		}
		//後続するエスケープされていない double quote を検索する
		const char* p2 = p1 + 1;
		while( true ) {
			p2 = std::find( p2, span.End(), '"' ); 
			if( p2 == span.End() )
				return false;
			if( p2[-1] != '\\' )
				break;
			++p2;
		}
		//両方見つかれば検索成功
		result.Top() = p1;
		result.End() = p2 + 1;
		return true;
	}

	//　(def... に続くシンボル（つまり関数名など）を検出する
	static bool DefinitionFinder( const TextSpan& span, const char* pTarget,
								  TextSpan& result, const char*& className ) {
		const DefEntry* pDefTop = s_deftbl;
		const DefEntry* pDefEnd = s_deftbl + count_of(s_deftbl);
		const char* p1 = span.Top();
		pTarget = "(def";
		while( p1 < span.End() ) {
			// 次の (def で始まる箇所を検索 → みつからなければ終了
			p1 = std::search( p1, span.End(), pTarget, pTarget + 4 );
			if( p1 == span.End() )
				return false;
			// (def... の終端を検索（空白類文字の検索でいいかな）
			auto p2 = std::find_if( p1, span.End(),
									[]( char c ) -> bool { return c == ' ' || c == 0x09; } );
			// 見つけたものを Entry の一覧から検索（合致しなければループ継続）
			TextSpan tmp{ p1 + 1, p2 };
			auto pEntry = std::find_if( pDefTop, pDefEnd,
										[&tmp]( const DefEntry& def ) -> bool {
											return tmp.IsEqual( def.type ); } );
			if( pEntry == pDefEnd ) {
				p1 = p2;
				continue;
			}
			// 残り文字列から先頭の空白類文字を除去 → 空文字列になってしまったら終了
			TextSpan tmp2{ p2, span.End() };
			tmp2 = tmp2.TrimHead();
			if( tmp.IsEmpty() )
				return false;
			// シンボル構成文字の終端を検索 → そもそもシンボル構成文字がなければループ継続
			auto p = std::find_if( tmp2.Top(), tmp2.End(),
								   []( char c ) -> bool {
									   return !::strchr( SYMBOL_CHAR, c ); } );
			if( p == tmp2.Top() ) {
				p1 = p;
				continue;
			}

			result = TextSpan{ tmp2.Top(), p };
			className = pEntry->clsName;
			return true;
		}
		return false;
	}

	static bool GensymFinder( const TextSpan& span, const char* pTarget,
							  TextSpan& result, const char*& className ) {
		(void)className;
		// #:gensym 形式のキーワードシンボルを探すためにまずは "#:" を検索
		pTarget = "#:";
		auto p1 = std::search( span.Top(), span.End(), pTarget, pTarget + 2 );
		if( p1 == span.End() )
			return false;

		// その手前にシンボル構成文字があれば対象外
		if( span.Top() < p1 && !!::strchr( SYMBOL_CHAR, p1[-1] ) )
			return false;
		// その直後にシンボル構成文字がなければ対象外
		if( p1 == span.End() || !::strchr( SYMBOL_CHAR, p1[2] ) )
			return false;

		auto p2 = std::find_if( p1 + 2, span.End(),
								[]( char c ) -> bool {
									return !::strchr( SYMBOL_CHAR, c ); } );
		result.Top() = p1;
		result.End() = p2;
		return true;
	}

	static bool KeywordSymbolFinder( const TextSpan& span, const char* pTarget,
									 TextSpan& result, const char*& className ) {
		(void)pTarget;
		(void)className;
		// :keyword 形式のキーワードシンボルを探すためにまずは ':' を検索
		auto p1 = std::find( span.Top(), span.End(), ':' );
		if( p1 == span.End() )
			return false;

		// その手前にシンボル構成文字があれば対象外
		if( span.Top() < p1 && !!::strchr( SYMBOL_CHAR, p1[-1] ) )
			return false;
		// その直後にシンボル構成文字がなければ対象外
		if( p1 == span.End() || !::strchr( SYMBOL_CHAR, p1[1] ) )
			return false;

		auto p2 = std::find_if( p1 + 1, span.End(),
								[]( char c ) -> bool {
									return !::strchr( SYMBOL_CHAR, c ); } );
		result.Top() = p1;
		result.End() = p2;
		return true;
	}

	static bool KeywordFinder( const TextSpan& span, const char* pTarget,
							   TextSpan& result, const char*& className ) {
		(void)className;
		uint32_t len = ::strlen( pTarget );
		const char* p1 = span.Top();
		const char* p2 = nullptr;

		while( true ) {
			p1 = std::search( p1, span.End(), pTarget, pTarget + len );
			if( p1 == span.End() )
				return false;
			p2 = p1 + len;
			if( (p1 == span.Top() || !::strchr( SYMBOL_CHAR, p1[-1] )) &&
				(p2 == span.End() || !::strchr( SYMBOL_CHAR, p2[0]  )) )
				break;
			p1 += len;
		}
		result.Top() = p1;
		result.End() = p1 + len;
		return true;
	}


} // namespace turnup
