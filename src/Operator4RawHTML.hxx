//------------------------------------------------------------------------------
//
// Operator4RawHTML.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_RAW_HTML_HXX__
#define OPERATOR_4_RAW_HTML_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4RawHTML( const TextSpan* pTop,
									  const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_RAW_HTML_HXX__
