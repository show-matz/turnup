//------------------------------------------------------------------------------
//
// StylePalette.hxx
//
//------------------------------------------------------------------------------
#ifndef STYLEPALETTE_HXX__
#define STYLEPALETTE_HXX__

#include <stdint.h>

namespace turnup {

    class TextSpan;

    //--------------------------------------------------------------------------
    //
    // class StylePalette
    //
    //--------------------------------------------------------------------------
    class StylePalette {
    public:
        StylePalette();
        ~StylePalette();
    public:
        void RegisterStyle( uint32_t index, const TextSpan& style );
        const TextSpan* GetStyle( uint32_t index ) const;
    private:
        class Impl;
        Impl* m_pImpl;
    };

} // namespace turnup

#endif // STYLEPALETTE_HXX__
