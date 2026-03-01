//------------------------------------------------------------------------------
//
// InternalFilter4Attach.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Attach.hxx"

#include "StyleStack.hxx"
#include "DocumentInfo.hxx"
#include "TextSpan.hxx"
#include "Base64.hxx"
#include "File.hxx"

#include <iostream>

namespace turnup {

    bool InternalFilter4Attach( std::ostream& os, DocumentInfo& docInfo,
                                const TextSpan* pTop, const TextSpan* pEnd ) {
        auto& styles = docInfo.Get<StyleStack>();
        styles.WriteOpenTag( os, "ul" ) << std::endl;
        for( ; pTop < pEnd; ++pTop ) {
            const TextSpan& file = *pTop;
            os << "  "; styles.WriteOpenTag( os, "li" );
            if( File::IsExist( file ) == false  ) {
                std::cerr << "ERROR : file " << file << " is not exist." << std::endl;
                os << file;
            } else {
                os << "<a href=\"";
                os << "data:" << Base64::GetMimeType( file ) << ";base64,";
                Base64::EncodeFile( os, file );
                os << "\" download=\"" << file << "\">" << file << "</a>";
            }
            os << "</li>" << std::endl;
        }
        os << "</ul>" << std::endl;
        return true;
    }

} // namespace turnup
