//------------------------------------------------------------------------------
//
// Operator4EmptyLine.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4EMPTYLINE_HXX_
#define SRC_OPERATOR4EMPTYLINE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4EmptyLine(const TextSpan* pTop,
                                   const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4EMPTYLINE_HXX_
