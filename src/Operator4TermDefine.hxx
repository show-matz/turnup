//------------------------------------------------------------------------------
//
// Operator4TermDefine.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4TERMDEFINE_HXX_
#define SRC_OPERATOR4TERMDEFINE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

bool IsTermDefine(const TextSpan& line,
                  const char*& pTop, const char*& pEnd);
const TextSpan* Operator4TermDefine(const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4TERMDEFINE_HXX_
