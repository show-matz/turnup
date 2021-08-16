//------------------------------------------------------------------------------
//
// InternalFilter4CandCxx.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4CandCxx.hxx"

#include "InternalFilter.hxx"
#include "TextSpan.hxx"
#include "FilterBuffer.hxx"

#include <iostream>
#include <algorithm>
#include <string.h>

namespace turnup {

	bool RangeCommentFinder( const TextSpan& span,
							 const char* pTarget, TextSpan& result );
	bool LineCommentFinder( const TextSpan& span,
							const char* pTarget, TextSpan& result );
	bool StringLiteralFinder( const TextSpan& span,
							  const char* pTarget, TextSpan& result );
	bool KeywordFinder( const TextSpan& span,
						const char* pTarget, TextSpan& result );

	static const RangeFinderUnit s_units4C[] = {
		{ RangeCommentFinder,	nullptr,    "comment"  },
		{ LineCommentFinder,	nullptr,    "comment"  },
		{ StringLiteralFinder,	nullptr,    "literal"  },
		{ KeywordFinder,		"auto",     "keyword2" },
		{ KeywordFinder,		"char",     "keyword2" },
		{ KeywordFinder,		"break",    "keyword1" },
		{ KeywordFinder,		"case",     "keyword1" },
		{ KeywordFinder,		"const",    "keyword1" },
		{ KeywordFinder,		"continue", "keyword1" },
		{ KeywordFinder,		"default",  "keyword1" },
		{ KeywordFinder,		"do",       "keyword1" },
		{ KeywordFinder,		"double",   "keyword2" },
		{ KeywordFinder,		"else",     "keyword1" },
		{ KeywordFinder,		"enum",     "keyword1" },
		{ KeywordFinder,		"extern",   "keyword1" },
		{ KeywordFinder,		"float",    "keyword2" },
		{ KeywordFinder,		"for",      "keyword1" },
		{ KeywordFinder,		"goto",     "keyword1" },
		{ KeywordFinder,		"if",       "keyword1" },
		{ KeywordFinder,		"int",      "keyword2" },
		{ KeywordFinder,		"long",     "keyword2" },
		{ KeywordFinder,		"register", "keyword1" },
		{ KeywordFinder,		"return",   "keyword1" },
		{ KeywordFinder,		"signed",   "keyword2" },
		{ KeywordFinder,		"sizeof",   "keyword1" },
		{ KeywordFinder,		"short",    "keyword1" },
		{ KeywordFinder,		"static",   "keyword1" },
		{ KeywordFinder,		"struct",   "keyword1" },
		{ KeywordFinder,		"switch",   "keyword1" },
		{ KeywordFinder,		"typedef",  "keyword1" },
		{ KeywordFinder,		"union",    "keyword1" },
		{ KeywordFinder,		"unsigned", "keyword2" },
		{ KeywordFinder,		"void",     "keyword2" },
		{ KeywordFinder,		"volatile", "keyword1" }, 
		{ KeywordFinder,		"while",    "keyword1" }, 
		// preprocessor directives
		{ KeywordFinder,		"#define",  "keyword3" },
		{ KeywordFinder,		"#elif",    "keyword3" },
		{ KeywordFinder,		"#else",    "keyword3" },
		{ KeywordFinder,		"#endif",   "keyword3" },
		{ KeywordFinder,		"#error",   "keyword3" },
		{ KeywordFinder,		"#if",      "keyword3" },
		{ KeywordFinder,		"#ifdef",   "keyword3" },
		{ KeywordFinder,		"#ifndef",  "keyword3" },
		{ KeywordFinder,		"#import",  "keyword3" },
		{ KeywordFinder,		"#include", "keyword3" },
		{ KeywordFinder,		"#line",    "keyword3" },
		{ KeywordFinder,		"#pragma",  "keyword3" },
		{ KeywordFinder,		"#undef",   "keyword3" },
		{ KeywordFinder,		"#using",   "keyword3" },
		// from stdint.h
		{ KeywordFinder,		"int8_t",         "keyword2" },
		{ KeywordFinder,		"int16_t",        "keyword2" },
		{ KeywordFinder,		"int32_t",        "keyword2" },
		{ KeywordFinder,		"int64_t",        "keyword2" },
		{ KeywordFinder,		"int_fast8_t",    "keyword2" },
		{ KeywordFinder,		"int_fast16_t",   "keyword2" },
		{ KeywordFinder,		"int_fast32_t",   "keyword2" },
		{ KeywordFinder,		"int_fast64_t",   "keyword2" },
		{ KeywordFinder,		"int_least8_t",   "keyword2" },
		{ KeywordFinder,		"int_least16_t",  "keyword2" },
		{ KeywordFinder,		"int_least32_t",  "keyword2" },
		{ KeywordFinder,		"int_least64_t",  "keyword2" },
		{ KeywordFinder,		"uint8_t",        "keyword2" },
		{ KeywordFinder,		"uint16_t",       "keyword2" },
		{ KeywordFinder,		"uint32_t",       "keyword2" },
		{ KeywordFinder,		"uint64_t",       "keyword2" },
		{ KeywordFinder,		"uint_fast8_t",   "keyword2" },
		{ KeywordFinder,		"uint_fast16_t",  "keyword2" },
		{ KeywordFinder,		"uint_fast32_t",  "keyword2" },
		{ KeywordFinder,		"uint_fast64_t",  "keyword2" },
		{ KeywordFinder,		"uint_least8_t",  "keyword2" },
		{ KeywordFinder,		"uint_least16_t", "keyword2" },
		{ KeywordFinder,		"uint_least32_t", "keyword2" },
		{ KeywordFinder,		"uint_least64_t", "keyword2" },
		{ KeywordFinder,		"intmax_t",       "keyword2" },
		{ KeywordFinder,		"intptr_t",       "keyword2" },
		{ KeywordFinder,		"uintmax_t",      "keyword2" },
		{ KeywordFinder,		"uintptr_t",      "keyword2" },
		// endmark
		{ nullptr,				nullptr,    nullptr   }
	};

	static const RangeFinderUnit s_units4Cxx[] = {
		{ RangeCommentFinder,	nullptr,            "comment"  },
		{ LineCommentFinder,	nullptr,            "comment"  },
		{ StringLiteralFinder,	nullptr,            "literal"  },
		{ KeywordFinder,		"alignas",          "keyword1" },
		{ KeywordFinder,		"alignof",          "keyword1" },
		{ KeywordFinder,		"asm",              "keyword1" },
		{ KeywordFinder,		"auto",             "keyword2" },
		{ KeywordFinder,		"bool",             "keyword2" },
		{ KeywordFinder,		"break",            "keyword1" },
		{ KeywordFinder,		"case",             "keyword1" },
		{ KeywordFinder,		"catch",            "keyword1" },
		{ KeywordFinder,		"char",             "keyword2" },
		{ KeywordFinder,		"char16_t",         "keyword2" },
		{ KeywordFinder,		"char32_t",         "keyword2" },
		{ KeywordFinder,		"class",            "keyword1" },
		{ KeywordFinder,		"const",            "keyword1" },
		{ KeywordFinder,		"const_cast",       "keyword1" },
		{ KeywordFinder,		"constexpr",        "keyword1" },
		{ KeywordFinder,		"continue",         "keyword1" },
		{ KeywordFinder,		"decltype",         "keyword1" },
		{ KeywordFinder,		"default",          "keyword1" },
		{ KeywordFinder,		"delete",           "keyword1" },
		{ KeywordFinder,		"do",               "keyword1" },
		{ KeywordFinder,		"double",           "keyword2" },
		{ KeywordFinder,		"dynamic_cast",     "keyword1" },
		{ KeywordFinder,		"else",             "keyword1" },
		{ KeywordFinder,		"enum",             "keyword1" },
		{ KeywordFinder,		"explicit",         "keyword1" },
		{ KeywordFinder,		"export",           "keyword1" },
		{ KeywordFinder,		"extern",           "keyword1" },
		{ KeywordFinder,		"false",            "literal"  },
		{ KeywordFinder,		"final",            "keyword1" },
		{ KeywordFinder,		"float",            "keyword2" },
		{ KeywordFinder,		"for",              "keyword1" },
		{ KeywordFinder,		"friend",           "keyword1" },
		{ KeywordFinder,		"goto",             "keyword1" },
		{ KeywordFinder,		"if",               "keyword1" },
		{ KeywordFinder,		"inline",           "keyword1" },
		{ KeywordFinder,		"int",              "keyword2" },
		{ KeywordFinder,		"long",             "keyword2" },
		{ KeywordFinder,		"mutable",          "keyword1" },
		{ KeywordFinder,		"namespace",        "keyword1" },
		{ KeywordFinder,		"new",              "keyword1" },
		{ KeywordFinder,		"noexcept",         "keyword1" },
		{ KeywordFinder,		"nullptr",          "literal"  },
		{ KeywordFinder,		"operator",         "keyword1" },
		{ KeywordFinder,		"override",         "keyword1" },
		{ KeywordFinder,		"private",          "keyword1" },
		{ KeywordFinder,		"protected",        "keyword1" },
		{ KeywordFinder,		"public",           "keyword1" },
		{ KeywordFinder,		"register",         "keyword1" },
		{ KeywordFinder,		"reinterpret_cast", "keyword1" },
		{ KeywordFinder,		"return",           "keyword1" },
		{ KeywordFinder,		"short",            "keyword2" },
		{ KeywordFinder,		"signed",           "keyword2" },
		{ KeywordFinder,		"sizeof",           "keyword1" },
		{ KeywordFinder,		"static",           "keyword1" },
		{ KeywordFinder,		"static_assert",    "keyword1" },
		{ KeywordFinder,		"static_cast",      "keyword1" },
		{ KeywordFinder,		"struct",           "keyword1" },
		{ KeywordFinder,		"switch",           "keyword1" },
		{ KeywordFinder,		"template",         "keyword1" },
		{ KeywordFinder,		"this",             "keyword1" },
		{ KeywordFinder,		"thread_local",     "keyword1" },
		{ KeywordFinder,		"throw",            "keyword1" },
		{ KeywordFinder,		"true",             "literal"  },
		{ KeywordFinder,		"try",              "keyword1" },
		{ KeywordFinder,		"typedef",          "keyword1" },
		{ KeywordFinder,		"typeid",           "keyword1" },
		{ KeywordFinder,		"typename",         "keyword1" },
		{ KeywordFinder,		"union",            "keyword1" },
		{ KeywordFinder,		"unsigned",         "keyword2" },
		{ KeywordFinder,		"using",            "keyword1" },
		{ KeywordFinder,		"virtual",          "keyword1" },
		{ KeywordFinder,		"void",             "keyword2" },
		{ KeywordFinder,		"volatile",         "keyword1" },
		{ KeywordFinder,		"wchar_t",          "keyword2" },
		{ KeywordFinder,		"while",            "keyword1" },
		// preprocessor directives
		{ KeywordFinder,		"#define",          "keyword3" },
		{ KeywordFinder,		"#elif",            "keyword3" },
		{ KeywordFinder,		"#else",            "keyword3" },
		{ KeywordFinder,		"#endif",           "keyword3" },
		{ KeywordFinder,		"#error",           "keyword3" },
		{ KeywordFinder,		"#if",              "keyword3" },
		{ KeywordFinder,		"#ifdef",           "keyword3" },
		{ KeywordFinder,		"#ifndef",          "keyword3" },
		{ KeywordFinder,		"#import",          "keyword3" },
		{ KeywordFinder,		"#include",         "keyword3" },
		{ KeywordFinder,		"#line",            "keyword3" },
		{ KeywordFinder,		"#pragma",          "keyword3" },
		{ KeywordFinder,		"#undef",           "keyword3" },
		{ KeywordFinder,		"#using",           "keyword3" },
		// from stdint.h        
		{ KeywordFinder,		"int8_t",           "keyword2" },
		{ KeywordFinder,		"int16_t",          "keyword2" },
		{ KeywordFinder,		"int32_t",          "keyword2" },
		{ KeywordFinder,		"int64_t",          "keyword2" },
		{ KeywordFinder,		"int_fast8_t",      "keyword2" },
		{ KeywordFinder,		"int_fast16_t",     "keyword2" },
		{ KeywordFinder,		"int_fast32_t",     "keyword2" },
		{ KeywordFinder,		"int_fast64_t",     "keyword2" },
		{ KeywordFinder,		"int_least8_t",     "keyword2" },
		{ KeywordFinder,		"int_least16_t",    "keyword2" },
		{ KeywordFinder,		"int_least32_t",    "keyword2" },
		{ KeywordFinder,		"int_least64_t",    "keyword2" },
		{ KeywordFinder,		"uint8_t",          "keyword2" },
		{ KeywordFinder,		"uint16_t",         "keyword2" },
		{ KeywordFinder,		"uint32_t",         "keyword2" },
		{ KeywordFinder,		"uint64_t",         "keyword2" },
		{ KeywordFinder,		"uint_fast8_t",     "keyword2" },
		{ KeywordFinder,		"uint_fast16_t",    "keyword2" },
		{ KeywordFinder,		"uint_fast32_t",    "keyword2" },
		{ KeywordFinder,		"uint_fast64_t",    "keyword2" },
		{ KeywordFinder,		"uint_least8_t",    "keyword2" },
		{ KeywordFinder,		"uint_least16_t",   "keyword2" },
		{ KeywordFinder,		"uint_least32_t",   "keyword2" },
		{ KeywordFinder,		"uint_least64_t",   "keyword2" },
		{ KeywordFinder,		"intmax_t",         "keyword2" },
		{ KeywordFinder,		"intptr_t",         "keyword2" },
		{ KeywordFinder,		"uintmax_t",        "keyword2" },
		{ KeywordFinder,		"uintptr_t",        "keyword2" },
		// endmark
		{ nullptr,				nullptr,            nullptr   }
	};

	//------------------------------------------------------------------------------
	//
	// exported functions
	//
	//------------------------------------------------------------------------------
	bool InternalFilter4C( std::ostream& os,
						   const TextSpan* pTop, const TextSpan* pEnd ) {
		FilterBuffer buf{ pTop, pEnd };
		TextSpan span = buf.GetBuffer();
		os << "<pre class='code'>" << std::endl;
		InternalFilter::ExecRecursive( os, span, s_units4C + 0 );
		os << "</pre>" << std::endl;
		return true;

	}

	bool InternalFilter4Cxx( std::ostream& os,
							 const TextSpan* pTop, const TextSpan* pEnd ) {
		FilterBuffer buf{ pTop, pEnd };
		TextSpan span = buf.GetBuffer();
		os << "<pre class='code'>" << std::endl;
		InternalFilter::ExecRecursive( os, span, s_units4Cxx + 0 );
		os << "</pre>" << std::endl;
		return true;

	}

	//------------------------------------------------------------------------------
	//
	// internal functions
	//
	//------------------------------------------------------------------------------
	bool RangeCommentFinder( const TextSpan& span,
							 const char* pTarget, TextSpan& result ) {
		//MEMO : 文字列中の /* とかを回避するのは現状では諦めてる．．．
		pTarget = "/*";
		auto p1 = std::search( span.Top(), span.End(), pTarget, pTarget + 2 );
		if( p1 == span.End() )
			return false;
		pTarget = "*/";
		auto p2 = std::search( p1+2, span.End(), pTarget, pTarget + 2 );
		if( p2 == span.End() )
			return false;
		result.Top() = p1;
		result.End() = p2 + 2;
		return true;
	}

	bool LineCommentFinder( const TextSpan& span,
							const char* pTarget, TextSpan& result ) {
		//MEMO : 文字列中の // とかを回避するのは現状では諦めてる．．．
		pTarget = "//";
		auto p1 = std::search( span.Top(), span.End(), pTarget, pTarget + 2 );
		if( p1 == span.End() )
			return false;
		auto p2 = std::find( p1, span.End(), 0x0A ); 
		result.Top() = p1;
		result.End() = p2;
		return true;
	}

	bool StringLiteralFinder( const TextSpan& span,
							  const char* pTarget, TextSpan& result ) {
		(void)pTarget;
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

	bool KeywordFinder( const TextSpan& span,
						const char* pTarget, TextSpan& result ) {
		static const char* IDCHAR = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
		uint32_t len = ::strlen( pTarget );
		const char* p1 = span.Top();
		const char* p2 = nullptr;

		while( true ) {
			p1 = std::search( p1, span.End(), pTarget, pTarget + len );
			if( p1 == span.End() )
				return false;
			p2 = p1 + len;
			if( (p1 == span.Top() || !::strchr( IDCHAR, p1[-1] )) &&
				(p2 == span.End() || !::strchr( IDCHAR, p2[0]  )) )
				break;
			p1 += len;
		}
		result.Top() = p1;
		result.End() = p1 + len;
		return true;
	}


} // namespace turnup
