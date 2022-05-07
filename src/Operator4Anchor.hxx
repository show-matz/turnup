//------------------------------------------------------------------------------
//
// Operator4Anchor.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_ANCHOR_HXX__
#define OPERATOR_4_ANCHOR_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4Anchor( const TextSpan* pTop,
									 const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_ANCHOR_HXX__
