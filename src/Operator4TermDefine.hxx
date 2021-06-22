//------------------------------------------------------------------------------
//
// Operator4TermDefine.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_TERMDEFINE_HXX__
#define OPERATOR_4_TERMDEFINE_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	bool IsTermDefine( const TextSpan& line,
					   const char*& pTop, const char*& pEnd );
	const TextSpan* Operator4TermDefine( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_TERMDEFINE_HXX__
