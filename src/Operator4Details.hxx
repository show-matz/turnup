//------------------------------------------------------------------------------
//
// Operator4Details.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4DETAILS_HXX_
#define SRC_OPERATOR4DETAILS_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Details(const TextSpan* pTop,
                                 const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4DETAILS_HXX_
