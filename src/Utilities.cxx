//------------------------------------------------------------------------------
//
// Utilities.cxx
//
//------------------------------------------------------------------------------
#include "Utilities.hxx"

using namespace std;

namespace turnup {

    static bool IsSpecificChar( char c, const uint8_t* p ) {
        if( c == ',' || c == ')' || c == '>' || c == ']' || c == '}' ||
            c == '.' || c == '(' || c == '<' || c == '[' || c == '{' )
            return true;
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0x8C ) return true;  // ，
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0x8E ) return true;  // ．
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x81 ) return true;  // 、
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x82 ) return true;  // 。
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x8C ) return true;  // 「
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x8D ) return true;  // 」
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x8E ) return true;  // 『
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x8F ) return true;  // 』
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0x88 ) return true;  // （
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0x89 ) return true;  // ）
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0x9C ) return true;  // ＜
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0x9E ) return true;  // ＞
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0xBB ) return true;  // ［
        if( p[0] == 0xEF && p[1] == 0xBC && p[2] == 0xBD ) return true;  // ］
        if( p[0] == 0xEF && p[1] == 0xBD && p[2] == 0x9B ) return true;  // ｛
        if( p[0] == 0xEF && p[1] == 0xBD && p[2] == 0x9D ) return true;  // ｝
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x8A ) return true;  // 《
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x8B ) return true;  // 》
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x88 ) return true;  // 〈
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x89 ) return true;  // 〉
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x90 ) return true;  // 【
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x91 ) return true;  // 】
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x94 ) return true;  // 〔
        if( p[0] == 0xE3 && p[1] == 0x80 && p[2] == 0x95 ) return true;  // 〕
        if( p[0] == 0xE2 && p[1] == 0x80 && p[2] == 0x9C ) return true;  // “
        if( p[0] == 0xE2 && p[1] == 0x80 && p[2] == 0x9D ) return true;  // ”
        if( p[0] == 0xE2 && p[1] == 0x80 && p[2] == 0x98 ) return true;  // ‘
        if( p[0] == 0xE2 && p[1] == 0x80 && p[2] == 0x99 ) return true;  // ’
        return false;
    }

    // `code` などの文字修飾の「直後」に来ることを許す（空白類以外の）文字の判定
    bool IsSpecificForward( const char* p ) {
        return IsSpecificChar( *p, reinterpret_cast<const uint8_t*>( p ) );
    }
    // `code` などの文字修飾の「直前」に来ることを許す（空白類以外の）文字の判定
    bool IsSpecificBackward( const char* p ) {
        return IsSpecificChar( *p, reinterpret_cast<const uint8_t*>( p - 2 ) );
    }

    //--------------------------------------------------------------------------
    //
    // implementation of class Utilities
    //
    //--------------------------------------------------------------------------
    void Utilities::Trim( const char*& p1, const char*& p2 ) {
        for( ; p1 < p2; ++p1 ) {
            if( *p1 != ' ' && *p1 != 0x09 )
                break;
        }
        for( ; p1 < p2; --p2 ) {
            if( p2[-1] != ' ' && p2[-1] != 0x09 )
                break;
        }
    }

}

