//------------------------------------------------------------------------------
//
// Operator4EmbedFootnotes.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4EMBEDFOOTNOTES_HXX_
#define SRC_OPERATOR4EMBEDFOOTNOTES_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4EmbedFootnotes(const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4EMBEDFOOTNOTES_HXX_
