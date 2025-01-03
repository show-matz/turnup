//------------------------------------------------------------------------------
//
// Operator4StylePalette.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_STYLEPALETTE_HXX__
#define OPERATOR_4_STYLEPALETTE_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4StylePalette( const TextSpan* pTop,
                                           const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_STYLEPALETTE_HXX__
