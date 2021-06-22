//------------------------------------------------------------------------------
//
// Operator4NumberedList.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATOR_4_NUMBEREDLIST_HXX__
#define OPERATOR_4_NUMBEREDLIST_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;

	const TextSpan* Operator4NumberedList( const TextSpan* pTop,
										   const TextSpan* pEnd, DocumentInfo& docInfo );

} // namespace turnup

#endif // OPERATOR_4_NUMBEREDLIST_HXX__
