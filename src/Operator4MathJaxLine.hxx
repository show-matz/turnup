//------------------------------------------------------------------------------
//
// Operator4MathJaxLine.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4MATHJAXLINE_HXX_
#define SRC_OPERATOR4MATHJAXLINE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4MathJaxLine(const TextSpan* pTop,
                                     const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4MATHJAXLINE_HXX_
