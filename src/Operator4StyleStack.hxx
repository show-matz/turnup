//------------------------------------------------------------------------------
//
// Operator4StyleStack.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4STYLESTACK_HXX_
#define SRC_OPERATOR4STYLESTACK_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4StyleStack(const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4STYLESTACK_HXX_
