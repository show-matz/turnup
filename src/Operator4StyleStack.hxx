//------------------------------------------------------------------------------
//
// Operator4StyleStack.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_STYLESTACK_HXX__
#define OPERATOR_4_STYLESTACK_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4StyleStack( const TextSpan* pTop,
                                         const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_STYLESTACK_HXX__
