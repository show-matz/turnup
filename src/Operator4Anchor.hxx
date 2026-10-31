//------------------------------------------------------------------------------
//
// Operator4Anchor.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4ANCHOR_HXX_
#define SRC_OPERATOR4ANCHOR_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Anchor(const TextSpan* pTop,
                                const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4ANCHOR_HXX_
