//------------------------------------------------------------------------------
//
// CRC64.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_CRC64_HXX_
#define SRC_CRC64_HXX_

#include <stdint.h>

namespace turnup {

//--------------------------------------------------------------------------
//
// class utility CRC64
//
//--------------------------------------------------------------------------
class CRC64 {
 public:
    static void RegisterSalt(const char* pSalt);
    static uint64_t Calc(char type,
                         const uint8_t* (*next)(void*),
                         void* pOpaque, char* pTagBuf = nullptr);
    static uint64_t Calc(char type, const char* pTop,
                         const char* pEnd, char* pTagBuf = nullptr);
};

}  // namespace turnup

#endif  // SRC_CRC64_HXX_

