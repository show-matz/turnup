//------------------------------------------------------------------------------
//
// Operator4EmbedIndex.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_EMBEDINDEX_HXX__
#define OPERATOR_4_EMBEDINDEX_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4EmbedIndex( const TextSpan* pTop,
                                         const TextSpan* pEnd, DocumentInfo& docInfo );
    const TextSpan* Operator4EmbedIndex_X( const TextSpan* pTop,
                                           const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_EMBEDINDEX_HXX__
