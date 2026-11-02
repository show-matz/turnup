//------------------------------------------------------------------------------
//
// Operator4EmbedIndex.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4EMBEDINDEX_HXX_
#define SRC_OPERATOR4EMBEDINDEX_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4EmbedIndex(const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo);
const TextSpan* Operator4EmbedIndex_X(const TextSpan* pTop,
                                      const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4EMBEDINDEX_HXX_
