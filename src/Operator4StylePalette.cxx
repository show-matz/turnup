//------------------------------------------------------------------------------
//
// Operator4StylePalette.cxx
//
//------------------------------------------------------------------------------
#include "Operator4StylePalette.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "StylePalette.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

	const TextSpan* Operator4StylePalette( const TextSpan* pTop,
										   const TextSpan* pEnd, DocumentInfo& docInfo ) {
		(void)pEnd;

		// <!-- palette: で始まっていない行は対象外なので無視
		if( pTop->BeginWith( "<!-- palette:" ) == false )
			return nullptr;

		// --> で終わっていない行も対象外なので無視
		TextSpan line = pTop->TrimTail();
		if( line.EndWith( "-->" ) == false )
			return nullptr;

		// 先頭の <!-- palette: と 末尾の --> を除去して更に Trim
		line = line.Chomp( 13, 3 ).Trim();

		// palette: に後続する type を取得（ 0~9 を期待 : そうでないなら無視）
		TextSpan idx = line.CutNextToken( ' ' );
		line = line.Trim();
		if( idx.ByteLength() != 1 || !('0' <= idx[0] && idx[0] <= '9') )
			return nullptr;

		StylePalette& palette = docInfo.Get<StylePalette>();

		palette.RegisterStyle( idx[0] - '0', line );
		return pTop + 1;
	}

} // namespace turnup

