//------------------------------------------------------------------------------
//
// Operator4FigureAndTable.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_FIGUREANDTABLE_HXX__
#define OPERATOR_4_FIGUREANDTABLE_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4FigureAndTable( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_FIGUREANDTABLE_HXX__
