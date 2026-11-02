//------------------------------------------------------------------------------
//
// Operator4Error.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4ERROR_HXX_
#define SRC_OPERATOR4ERROR_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Error(const TextSpan* pTop,
                               const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4ERROR_HXX_
