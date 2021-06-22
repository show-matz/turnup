//------------------------------------------------------------------------------
//
// Operator4EmbedFootnotes.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_EMBEDFOOTNOTES_HXX__
#define OPERATOR_4_EMBEDFOOTNOTES_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4EmbedFootnotes( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_EMBEDFOOTNOTES_HXX__
