//------------------------------------------------------------------------------
//
// Operator4Image.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4IMAGE_HXX_
#define SRC_OPERATOR4IMAGE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Image(const TextSpan* pTop,
                               const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4IMAGE_HXX_
