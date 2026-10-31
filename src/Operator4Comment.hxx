//------------------------------------------------------------------------------
//
// Operator4Comment.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4COMMENT_HXX_
#define SRC_OPERATOR4COMMENT_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Comment(const TextSpan* pTop,
                                 const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4COMMENT_HXX_
