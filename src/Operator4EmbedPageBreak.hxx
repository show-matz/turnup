//------------------------------------------------------------------------------
//
// Operator4EmbedPageBreak.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_EMBEDPAGEBREAK_HXX__
#define OPERATOR_4_EMBEDPAGEBREAK_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4EmbedPageBreak( const TextSpan* pTop,
                                             const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_EMBEDPAGEBREAK_HXX__
