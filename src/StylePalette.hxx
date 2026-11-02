//------------------------------------------------------------------------------
//
// StylePalette.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_STYLEPALETTE_HXX_
#define SRC_STYLEPALETTE_HXX_

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
    void RegisterStyle(uint32_t index, const TextSpan& style);
    const TextSpan* GetStyle(uint32_t index) const;

 private:
    class Impl;
    Impl* m_pImpl;
};

}  // namespace turnup

#endif  // SRC_STYLEPALETTE_HXX_
