//------------------------------------------------------------------------------
//
// Operator4Paragraph.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4PARAGRAPH_HXX_
#define SRC_OPERATOR4PARAGRAPH_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Paragraph(const TextSpan* pTop,
                                   const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4PARAGRAPH_HXX_
