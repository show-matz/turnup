//------------------------------------------------------------------------------
//
// Operator4HorizontalLine.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4HORIZONTALLINE_HXX_
#define SRC_OPERATOR4HORIZONTALLINE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4HorizontalLine(const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4HORIZONTALLINE_HXX_
