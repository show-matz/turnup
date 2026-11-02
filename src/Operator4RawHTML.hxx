//------------------------------------------------------------------------------
//
// Operator4RawHTML.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4RAWHTML_HXX_
#define SRC_OPERATOR4RAWHTML_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4RawHTML(const TextSpan* pTop,
                                 const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4RAWHTML_HXX_
