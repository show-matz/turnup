//------------------------------------------------------------------------------
//
// Operator4Comment.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_COMMENT_HXX__
#define OPERATOR_4_COMMENT_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4Comment( const TextSpan* pTop,
                                      const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_COMMENT_HXX__
