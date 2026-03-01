//------------------------------------------------------------------------------
//
// Base64.hxx
//
//------------------------------------------------------------------------------
#ifndef BASE64_HXX__
#define BASE64_HXX__

#include <cstdint>
#include <iosfwd>

namespace turnup {

	class TextSpan;

    //--------------------------------------------------------------------------
    //
    // utility class Base64
    //
    //--------------------------------------------------------------------------
    class Base64 {
    public:
        Base64() = delete;
        Base64( const Base64& ) = delete;
        Base64& operator=( const Base64& ) = delete;
        ~Base64() = delete;
    private:
        static constexpr const char* CODE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz"
                                            "0123456789+/";
    public:
        static const char* GetMimeType( const TextSpan& fileName );
        static void EncodeFile( std::ostream& os, const TextSpan& fileName );
    public:
        // Do NOT call when length == 0.
        static inline const char* Encode( uint8_t*& pBuf, uint32_t& length ) {
            uint32_t value = (                pBuf[0]      << 24) |
                             (((1 < length) ? pBuf[1] : 0) << 16) |
                             (((2 < length) ? pBuf[2] : 0) <<  8);
            static char buf[5];
            buf[0] = CODE[ (value >> 26) & 0x3F ];
            buf[1] = CODE[ (value >> 20) & 0x3F ];
            buf[2] = CODE[ (value >> 14) & 0x3F ];
            buf[3] = CODE[ (value >>  8) & 0x3F ];
            uint32_t n = (3 < length) ? 3 : length;
            switch( n ) {
            case 1: buf[2]  = '=';
                    // fall through.
            case 2: buf[3]  = '=';
                    // fall through.
            case 3: buf[4]  = 0;
                    pBuf   += n;
                    length -= n;
            }
            return buf;
        };
    };

}    // namespace turnup

#endif // BASE64_HXX__
