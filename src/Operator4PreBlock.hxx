//------------------------------------------------------------------------------
//
// Operator4PreBlock.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_PREBLOCK_HXX__
#define OPERATOR_4_PREBLOCK_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4PreBlock( const TextSpan* pTop,
                                       const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_PREBLOCK_HXX__
