//------------------------------------------------------------------------------
//
// Operator4Quote.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4QUOTE_HXX_
#define SRC_OPERATOR4QUOTE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Quote(const TextSpan* pTop,
                               const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4QUOTE_HXX_
