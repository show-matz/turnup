//------------------------------------------------------------------------------
//
// Operator4EmbedTOC.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_EMBEDTOC_HXX__
#define OPERATOR_4_EMBEDTOC_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4EmbedTOC( const TextSpan* pTop,
									   const TextSpan* pEnd, DocumentInfo& docInfo );
	const TextSpan* Operator4EmbedTableList( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo );
	const TextSpan* Operator4EmbedFigureList( const TextSpan* pTop,
											  const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_EMBEDTOC_HXX__
