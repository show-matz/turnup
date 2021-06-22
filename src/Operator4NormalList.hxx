//------------------------------------------------------------------------------
//
// Operator4NormalList.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_NORMALLIST_HXX__
#define OPERATOR_4_NORMALLIST_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4NormalList( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_NORMALLIST_HXX__
