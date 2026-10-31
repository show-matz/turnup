//------------------------------------------------------------------------------
//
// Operator4Table.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4TABLE_HXX_
#define SRC_OPERATOR4TABLE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4Table(const TextSpan* pTop,
                               const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4TABLE_HXX_
