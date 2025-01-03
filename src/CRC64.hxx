//------------------------------------------------------------------------------
//
// CRC64.hxx
//
//------------------------------------------------------------------------------
#ifndef CRC64_HXX__
#define CRC64_HXX__

#include <stdint.h>

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class utility CRC64
    //
    //--------------------------------------------------------------------------
    class CRC64 {
    public:
        static void RegisterSalt( const char* pSalt );
        static uint64_t Calc( char type, const char* pTop,
                              const char* pEnd, char* pTagBuf = nullptr );
    };

} // namespace turnup

#endif // CRC64_HXX__

