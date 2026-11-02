//------------------------------------------------------------------------------
//
// Operator4StylePalette.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4STYLEPALETTE_HXX_
#define SRC_OPERATOR4STYLEPALETTE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4StylePalette(const TextSpan* pTop,
                                      const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4STYLEPALETTE_HXX_
