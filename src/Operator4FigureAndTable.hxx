//------------------------------------------------------------------------------
//
// Operator4FigureAndTable.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATOR4FIGUREANDTABLE_HXX_
#define SRC_OPERATOR4FIGUREANDTABLE_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;

const TextSpan* Operator4FigureAndTable1(const TextSpan* pTop,
                                         const TextSpan* pEnd, DocumentInfo& docInfo);
const TextSpan* Operator4FigureAndTable2(const TextSpan* pTop,
                                         const TextSpan* pEnd, DocumentInfo& docInfo);

}  // namespace turnup

#endif  // SRC_OPERATOR4FIGUREANDTABLE_HXX_
