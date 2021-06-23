//------------------------------------------------------------------------------
//
// Operator4Comment.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Comment.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "Config.hxx"

#include <iostream>

namespace turnup {

	const TextSpan* Operator4Comment( const TextSpan* pTop,
									  const TextSpan* pEnd, DocumentInfo& docInfo ) {
		TextSpan tmp = pTop->Trim();
		if( tmp.BeginWith( "<!--" ) == false )
			return nullptr;
		bool bWrite = docInfo.Get<Config>().bWriteComment;
		while( pTop < pEnd ) {
			// 設定で明示的に指定されている場合のみコメントを出力
			if( bWrite ) {
				// TextSpan オブジェクトを ostream に渡すと文字エスケープなどをするのでここでは避ける
				std::cout.write( pTop->Top(), pTop->ByteLength() );
				std::cout << std::endl;
			}
			if( tmp.EndWith( "-->" ) )
				return pTop + 1;
			++pTop;
			tmp = pTop->Trim();
		}
		return pEnd;
	}


} // namespace turnup

