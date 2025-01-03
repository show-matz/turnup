//------------------------------------------------------------------------------
//
// Operator4EmptyLine.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_EMPTY_LINE_HXX__
#define OPERATOR_4_EMPTY_LINE_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4EmptyLine( const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_EMPTY_LINE_HXX__
