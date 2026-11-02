//------------------------------------------------------------------------------
//
// Operator4PreBlock.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4PREBLOCK_HXX_
#define SRC_OPERATOR4PREBLOCK_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4PreBlock(const TextSpan* pTop,
                                  const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4PREBLOCK_HXX_
