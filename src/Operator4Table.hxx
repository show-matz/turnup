//------------------------------------------------------------------------------
//
// Operator4Table.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_TABLE_HXX__
#define OPERATOR_4_TABLE_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4Table( const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_TABLE_HXX__
