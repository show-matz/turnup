//------------------------------------------------------------------------------
//
// InternalFilter4Shell.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Shell.hxx"

#include "InternalFilter.hxx"
#include "TextSpan.hxx"
#include "FilterBuffer.hxx"

#include <iostream>
#include <algorithm>
#include <string.h>

namespace turnup {

    static bool LineCommentFinder( const TextSpan& span, const char* pTarget,
                                   TextSpan& result, const char*& className );
    static bool StringLiteralFinder1( const TextSpan& span, const char* pTarget,
                                      TextSpan& result, const char*& className );
    static bool StringLiteralFinder2( const TextSpan& span, const char* pTarget,
                                      TextSpan& result, const char*& className );
    static bool StringLiteralFinderImpl( char chQuote, const TextSpan& span,
                                         const char* pTarget, TextSpan& result );
    static bool KeywordFinder( const TextSpan& span, const char* pTarget,
                               TextSpan& result, const char*& className );


    static const RangeFinderUnit s_units4Shell[] = {
        { LineCommentFinder,    nullptr,    "comment"  },
        { StringLiteralFinder2, nullptr,    "literal"  },
        { StringLiteralFinder1, nullptr,    "literal"  },
        { KeywordFinder,        "echo",     "keyword3" },
        { KeywordFinder,        "case",     "keyword1" },
        { KeywordFinder,        "do",       "keyword1" },
        { KeywordFinder,        "done",     "keyword1" },
        { KeywordFinder,        "elif",     "keyword1" },
        { KeywordFinder,        "else",     "keyword1" },
        { KeywordFinder,        "esac",     "keyword1" },
        { KeywordFinder,        "fi",       "keyword1" },
        { KeywordFinder,        "for",      "keyword1" },
        { KeywordFinder,        "function", "keyword1" },
        { KeywordFinder,        "if",       "keyword1" },
        { KeywordFinder,        "in",       "keyword1" },
        { KeywordFinder,        "select",   "keyword1" },
        { KeywordFinder,        "then",     "keyword1" },
        { KeywordFinder,        "until",    "keyword1" },
        { KeywordFinder,        "while",    "keyword1" },
        { KeywordFinder,        "time",     "keyword1" },
        // endmark
        { nullptr,              nullptr,    nullptr   }
    };


    //------------------------------------------------------------------------------
    //
    // exported functions
    //
    //------------------------------------------------------------------------------
    bool InternalFilter4Shell( std::ostream& os, DocumentInfo& docInfo,
                               const TextSpan* pTop, const TextSpan* pEnd ) {
        (void)docInfo;
        FilterBuffer buf{ pTop, pEnd };
        TextSpan span = buf.GetBuffer();
        os << "<pre class='code'>" << std::endl;
        InternalFilter::ExecRecursive( os, span, s_units4Shell + 0 );
        os << "</pre>" << std::endl;
        return true;

    }

    //------------------------------------------------------------------------------
    //
    // internal functions
    //
    //------------------------------------------------------------------------------
    static bool LineCommentFinder( const TextSpan& span, const char* pTarget,
                                   TextSpan& result, const char*& className ) {
        (void)className;
        //MEMO : 文字列中の # とかを回避するのは現状では諦めてる．．．
        pTarget = "#";
        auto p1 = std::search( span.Top(), span.End(), pTarget, pTarget + 1 );
        if( p1 == span.End() )
            return false;
        auto p2 = std::find( p1, span.End(), 0x0A ); 
        result.Top() = p1;
        result.End() = p2;
        return true;
    }

    static bool StringLiteralFinder1( const TextSpan& span, const char* pTarget,
                                      TextSpan& result, const char*& className ) {
        (void)className;
        return StringLiteralFinderImpl( '\'', span, pTarget, result );
    }

    static bool StringLiteralFinder2( const TextSpan& span, const char* pTarget,
                                      TextSpan& result, const char*& className ) {
        (void)className;
        return StringLiteralFinderImpl( '"', span, pTarget, result );
    }

    static bool StringLiteralFinderImpl( char chQuote, const TextSpan& span,
                                         const char* pTarget, TextSpan& result ) {
        (void)pTarget;
        //エスケープされていない quote 文字を検索する
        const char* p1 = span.Top();
        while( true ) {
            p1 = std::find( p1, span.End(), chQuote ); 
            if( p1 == span.End() )
                return false;
            if( p1[-1] != '\\' )
                break;
            ++p1;
        }
        //後続するエスケープされていない double quote を検索する
        const char* p2 = p1 + 1;
        while( true ) {
            p2 = std::find( p2, span.End(), chQuote ); 
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

    static bool KeywordFinder( const TextSpan& span, const char* pTarget,
                               TextSpan& result, const char*& className ) {
        (void)className;
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
