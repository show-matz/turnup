//------------------------------------------------------------------------------
//
// Operator4ImageModeStack.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_IMAGEMODESTACK_HXX__
#define OPERATOR_4_IMAGEMODESTACK_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4ImageModeStack( const TextSpan* pTop,
                                             const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_IMAGEMODESTACK_HXX__
