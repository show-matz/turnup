//------------------------------------------------------------------------------
//
// Operator4StyleStack.cxx
//
//------------------------------------------------------------------------------
#include "Operator4StyleStack.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "StyleStack.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

	const TextSpan* Operator4StyleStack( const TextSpan* pTop,
										 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		// <!-- stack: で始まっていない行は対象外なので無視
		if( pTop->BeginWith( "<!-- stack:" ) == false )
			return pTop;

		// --> で終わっていない行も対象外なので無視
		TextSpan line = pTop->TrimTail();
		if( line.EndWith( "-->" ) == false )
			return pTop;

		// 先頭の <!-- stack: と 末尾の --> を除去して更に Trim
		line = line.Chomp( 11, 3 ).Trim();

		// stack: に後続する type を取得（push/popを期待 : そうでないなら無視）
		TextSpan type = line.CutNextToken( ' ' );
		line = line.Trim();
		if( type.IsEqual( "push" ) == false && type.IsEqual( "pop" ) == false )
			return pTop;

		StyleStack& styles = docInfo.Get<StyleStack>();

		if( type.IsEqual( "push" ) ) {
			TextSpan tag = line.CutNextToken( ' ' );
			line = line.Trim();
			styles.PushStyle( tag, line );
		} else {
			if( styles.PopStyle( line ) == false ) {
				//ToDo : write error...?
			}
		}
		return pTop + 1;
	}

} // namespace turnup

