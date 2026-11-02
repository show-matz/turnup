//------------------------------------------------------------------------------
//
// Operator4EmbedTOC.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4EMBEDTOC_HXX_
#define SRC_OPERATOR4EMBEDTOC_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4EmbedTOC(const TextSpan* pTop,
                                  const TextSpan* pEnd, DocumentInfo& docInfo);
const TextSpan* Operator4EmbedTOC_X(const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo);
const TextSpan* Operator4EmbedSubTOC(const TextSpan* pTop,
                                     const TextSpan* pEnd, DocumentInfo& docInfo);
const TextSpan* Operator4EmbedTableList(const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo);
const TextSpan* Operator4EmbedFigureList(const TextSpan* pTop,
                                         const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4EMBEDTOC_HXX_
