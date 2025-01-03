//------------------------------------------------------------------------------
//
// Operator4Error.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_ERROR_HXX__
#define OPERATOR_4_ERROR_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4Error( const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_ERROR_HXX__
