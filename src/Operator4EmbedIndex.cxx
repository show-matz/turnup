//------------------------------------------------------------------------------
//
// Operator4EmbedIndex.cxx
//
//------------------------------------------------------------------------------
#include "Operator4EmbedIndex.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "Glossary.hxx"

#include <iostream>

namespace turnup {

	static const TextSpan* EmbedIndexImpl( bool bFoldable, const TextSpan* pTop,
										   const TextSpan* pEnd, DocumentInfo& docInfo );

	//--------------------------------------------------------------------------
	//
	// exported functions
	//	
	//--------------------------------------------------------------------------
	const TextSpan* Operator4EmbedIndex( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		return EmbedIndexImpl( false, pTop, pEnd, docInfo );
	}

	const TextSpan* Operator4EmbedIndex_X( const TextSpan* pTop,
										   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		return EmbedIndexImpl( true, pTop, pEnd, docInfo );
	}

	//--------------------------------------------------------------------------
	//
	// exported functions
	//	
	//--------------------------------------------------------------------------
	static const TextSpan* EmbedIndexImpl( bool bFoldable, const TextSpan* pTop,
										   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		const char* pHead = bFoldable ? "<!-- embed:index-x " : "<!-- embed:index ";

		// 対象外の行であれば無視
		if( pTop->BeginWith( pHead ) == false )
			return pTop;
		if( pTop->EndWith( " -->" ) == false )
			return pTop;

		{
			auto& glossary = docInfo.Get<Glossary>();
			glossary.WriteIndex( std::cout, docInfo, bFoldable );
		}
		return pTop + 1;
	}

} // namespace turnup

