//------------------------------------------------------------------------------
//
// Operator4Header.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4HEADER_HXX_
#define SRC_OPERATOR4HEADER_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Header(const TextSpan* pTop,
                                const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4HEADER_HXX_
