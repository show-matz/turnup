//------------------------------------------------------------------------------
//
// Operator4ImageModeStack.cxx
//
//------------------------------------------------------------------------------
#include "Operator4ImageModeStack.hxx"

#include "TextSpan.hxx"
#include "DocumentInfo.hxx"
#include "ImageModeStack.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

    const TextSpan* Operator4ImageModeStack( const TextSpan* pTop,
                                             const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;

        // <!-- image-mode: で始まっていない行は対象外なので無視
        if( pTop->BeginWith( "<!-- image-mode:" ) == false )
            return pTop;

        // --> で終わっていない行も対象外なので無視
        TextSpan line = pTop->TrimTail();
        if( line.EndWith( "-->" ) == false )
            return pTop;

        // 先頭の <!-- image-mode: と 末尾の --> を除去して更に Trim
        line = line.Chomp( 16, 3 ).Trim();

        // image-mode: に後続する type を取得（push/popを期待 : そうでないなら無視）
        TextSpan type = line.CutNextToken( ' ' );
        line = line.Trim();
        if( type.IsEqual( "push" ) == false && type.IsEqual( "pop" ) == false )
            return pTop;

        ImageModeStack& stack = docInfo.Get<ImageModeStack>();

        if( type.IsEqual( "push" ) ) {
            if( line.IsEqual( "link" ) )
                stack.PushMode( ImageMode::LINK );
            else if( line.IsEqual( "embed" ) )
                stack.PushMode( ImageMode::EMBED );
            else {
                //ToDo : error...?
            }
        } else {
            if( stack.PopMode() == false ) {
               //ToDo : error...?
            }
        }
        return pTop + 1;
    }

} // namespace turnup

