//------------------------------------------------------------------------------
//
// Operator4MathJaxLine.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_MATHJAXLINE_HXX__
#define OPERATOR_4_MATHJAXLINE_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4MathJaxLine( const TextSpan* pTop,
                                          const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_MATHJAXLINE_HXX__
