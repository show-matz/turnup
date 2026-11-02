//------------------------------------------------------------------------------
//
// Operator4ImageModeStack.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4IMAGEMODESTACK_HXX_
#define SRC_OPERATOR4IMAGEMODESTACK_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4ImageModeStack(const TextSpan* pTop,
                                        const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4IMAGEMODESTACK_HXX_
