//------------------------------------------------------------------------------
//
// Operator4Image.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Image.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "ImageModeStack.hxx"
#include "TextSpan.hxx"
#include "Base64.hxx"
#include "File.hxx"

#include <iostream>

namespace turnup {

    const TextSpan* Operator4Image( const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;
        TextSpan line = pTop->Trim();
        TextSpan alt;
        TextSpan url;
        if( line.IsMatch( "![", alt, "](", url, ")" ) == false )
            return pTop;

        auto& styles = docInfo.Get<StyleStack>();
        auto& mode   = docInfo.Get<ImageModeStack>();
        if( mode.GetCurrentMode() == ImageMode::EMBED ) {
            if( File::IsExist( url ) == false ) {
                //ファイルの実在チェックをする - NG なら終了
                std::cerr << "ERROR : file " << url << " is not exist." << std::endl;
                return pTop + 1;
            }
        }
        styles.WriteOpenTag( std::cout, "img",
                             " style='display: block; margin: auto;'", " src='" );
        if( mode.GetCurrentMode() == ImageMode::LINK ) {
            std::cout.write( url.Top(), url.ByteLength() );
        } else {
            std::cout << "data:" << Base64::GetMimeType( url ) << ";base64,";
            Base64::EncodeFile( std::cout, url );
            //ToDo : implement...  <img src='data:image/png;base64,iVBORw0K...U5ErkJggg==' />
        }
        std::cout << "' ";
        if( alt.IsEmpty() == false ) {
            std::cout << "alt='";
            alt.WriteTo( std::cout, docInfo );
            std::cout << "' ";
        }
        std::cout << "/>" << std::endl;
        return pTop + 1;
    }


} // namespace turnup

