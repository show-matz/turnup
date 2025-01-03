//------------------------------------------------------------------------------
//
// Operator4List.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_LIST_HXX__
#define OPERATOR_4_LIST_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4List( const TextSpan* pTop,
                                   const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_LIST_HXX__
