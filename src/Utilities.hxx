//------------------------------------------------------------------------------
//
// Utilities.hxx
//
//------------------------------------------------------------------------------
#ifndef UTILITIES_HXX__
#define UTILITIES_HXX__

#include <stdint.h>

namespace turnup {

    template <int N, typename T> 
    inline constexpr unsigned int count_of( T const (&)[N] ) { 
        return N; 
    }

    inline bool IsSpaceForward( const char* p ) {
        if( *p == 0x20 || *p == 0x09 || !*p )
            return true;
        auto q = reinterpret_cast<const uint8_t*>( p );
        if( q[0] == 0xE3 && q[1] == 0x80 && q[2] == 0x80 )
            return true;
        return false;
    }
    inline bool IsSpaceBackward( const char* p ) {
        if( *p == 0x20 || *p == 0x09 || !*p )
            return true;
        auto q = reinterpret_cast<const uint8_t*>( p );
        if( q[-2] == 0xE3 && q[-1] == 0x80 && q[0] == 0x80 )
            return true;
        return false;
    }

    //--------------------------------------------------------------------------
    //
    // class Utilities
    //
    //--------------------------------------------------------------------------
    class Utilities {
    public:
        Utilities() = delete;
        Utilities( const Utilities& ) = delete;
        Utilities& operator=( const Utilities& ) = delete;
        ~Utilities() = delete;
    public:
        static void Trim( const char*& p1, const char*& p2 );
    };

}

#endif // UTILITIES_HXX__
