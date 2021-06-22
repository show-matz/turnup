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
#include <iostream>

namespace turnup {

	const TextSpan* Operator4EmbedTOC( const TextSpan* pTop,
									   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		// 対象外の行であれば無視
		if( pTop->BeginWith( "<!-- embed:toc " ) == false )
			return nullptr;
		if( pTop->EndWith( " -->" ) == false )
			return nullptr;

		// min/max level 指定があるなら読み取る
		bool isError = false;
		TextSpan tmp{ *pTop };
		tmp = tmp.Chomp( 14, 4 ).Trim();
		uint32_t lvls[] = { 1, 6 };
		for( uint32_t i = 0; i < 2; ++i ) {
			if( tmp.IsEmpty() == false ) {
				TextSpan token = tmp.CutNextToken( ' ' );
				if( token.Convert( lvls[i] ) == false ) {
					std::cerr << "ERROR : invalid parameter for embed:toc." << std::endl;
					isError = true;
				}
			}
			tmp = tmp.Trim();
		}
		if( lvls[0] < 1 || 6 < lvls[1] || lvls[1] < lvls[0] ) {
			std::cerr << "ERROR : invalid parameter for embed:toc." << std::endl;
			isError = true;
		}
		if( !isError ) {
			auto& toc = docInfo.Get<ToC>();
			toc.WriteTOC( std::cout,
						  docInfo.Get<Config>(), lvls[0], lvls[1] );
		}
		return pTop + 1;
	}

} // namespace turnup

