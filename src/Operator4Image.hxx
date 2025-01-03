//------------------------------------------------------------------------------
//
// Operator4Image.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_IMAGE_HXX__
#define OPERATOR_4_IMAGE_HXX__

namespace turnup {

    class TextSpan;
    class DocumentInfo;

    const TextSpan* Operator4Image( const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_IMAGE_HXX__
