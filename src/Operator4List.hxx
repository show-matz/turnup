//------------------------------------------------------------------------------
//
// Operator4List.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4LIST_HXX_
#define SRC_OPERATOR4LIST_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4List(const TextSpan* pTop,
                              const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4LIST_HXX_
