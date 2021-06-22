//------------------------------------------------------------------------------
//
// Operator4Header.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_HEADER_HXX__
#define OPERATOR_4_HEADER_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4Header( const TextSpan* pTop,
									 const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_HEADER_HXX__
