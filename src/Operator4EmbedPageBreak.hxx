//------------------------------------------------------------------------------
//
// Operator4EmbedPageBreak.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4EMBEDPAGEBREAK_HXX_
#define SRC_OPERATOR4EMBEDPAGEBREAK_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4EmbedPageBreak(const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4EMBEDPAGEBREAK_HXX_
