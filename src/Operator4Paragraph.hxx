//------------------------------------------------------------------------------
//
// Operator4Paragraph.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_PARAGRAPH_HXX__
#define OPERATOR_4_PARAGRAPH_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4Paragraph( const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_PARAGRAPH_HXX__
