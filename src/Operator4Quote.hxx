//------------------------------------------------------------------------------
//
// Operator4Quote.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_QUOTE_HXX__
#define OPERATOR_4_QUOTE_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4Quote( const TextSpan* pTop,
									const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_QUOTE_HXX__
