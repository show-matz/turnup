//------------------------------------------------------------------------------
//
// Operator4EmbedTOC.cxx
//
//------------------------------------------------------------------------------
#include "Operator4EmbedTOC.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "ToC.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

	static const TextSpan* EmbedTocImpl( bool bFoldable, const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo );




	const TextSpan* Operator4EmbedTOC( const TextSpan* pTop,
									   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		return EmbedTocImpl( false, pTop, pEnd, docInfo );
	}

	const TextSpan* Operator4EmbedTOC_X( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		return EmbedTocImpl( true, pTop, pEnd, docInfo );
	}

	const TextSpan* Operator4EmbedTableList( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		// 対象外の行であれば無視
		if( pTop->IsEqual( "<!-- embed:table-list -->" ) == false )
			return pTop;
		auto& toc = docInfo.Get<ToC>();
		toc.WriteTableList( std::cout, docInfo );
		return pTop + 1;
	}

	const TextSpan* Operator4EmbedFigureList( const TextSpan* pTop,
											  const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;
		// 対象外の行であれば無視
		if( pTop->IsEqual( "<!-- embed:figure-list -->" ) == false )
			return pTop;
		auto& toc = docInfo.Get<ToC>();
		toc.WriteFigureList( std::cout, docInfo );
		return pTop + 1;
	}




	static const TextSpan* EmbedTocImpl( bool bFoldable, const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		const char* pHead = bFoldable ? "<!-- embed:toc-x " : "<!-- embed:toc ";

		// 対象外の行であれば無視
		if( pTop->BeginWith( pHead ) == false )
			return pTop;
		if( pTop->EndWith( " -->" ) == false )
			return pTop;

		// min/max level 指定があるなら読み取る
		bool isError = false;
		TextSpan tmp{ *pTop };
		tmp = tmp.Chomp( ::strlen( pHead ) - 1, 4 ).Trim();
		uint32_t lvls[] = { 1, 6 };
		for( uint32_t i = 0; i < 2; ++i ) {
			if( tmp.IsEmpty() == false ) {
				TextSpan token = tmp.CutNextToken( ' ' );
				if( token.Convert( lvls[i] ) == false ) {
					std::cerr << "ERROR : invalid parameter for "
										  << pHead + 5 << "." << std::endl;
					isError = true;
				}
			}
			tmp = tmp.Trim();
		}
		if( lvls[0] < 1 || 6 < lvls[1] || lvls[1] < lvls[0] ) {
			std::cerr << "ERROR : invalid parameter for "
					  						<< pHead + 5 << "." << std::endl;
			isError = true;
		}
		if( !isError ) {
			auto& toc = docInfo.Get<ToC>();
			toc.WriteTOC( std::cout, docInfo, bFoldable, lvls[0], lvls[1] );
		}
		return pTop + 1;
	}
} // Namespace turnup

