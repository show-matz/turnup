//------------------------------------------------------------------------------
//
// Operator4Details.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_DETAILS_HXX__
#define OPERATOR_4_DETAILS_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4Details( const TextSpan* pTop,
                                      const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_DETAILS_HXX__
