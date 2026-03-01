//------------------------------------------------------------------------------
//
// Base64.cxx
//
//------------------------------------------------------------------------------
#include "Base64.hxx"

#include "TextSpan.hxx"
#include "File.hxx"

#include <iostream>

namespace turnup {

    static const char* s_MimeTypes[] = {
        "png",      "image/png",
        "jpg",      "image/jpeg",
        "jpeg",     "image/jpeg",
        "gif",      "image/gif",
        "svg",      "image/svg+xml",
        "webp",     "image/webp",
        "txt",      "text/plain",
        "sh",       "text/plain",
        "md",       "text/plain",
        "html",     "text/html",
        "htm",      "text/html",
        "css",      "text/css",
        "js",       "text/javascript",
        "mp3",      "audio/mpeg",
        "wav",      "audio/wav",
        "mpeg",     "video/mpeg",
        "mp4",      "video/mp4",
        "webm",     "video/webm",
        "json",     "application/json",
        "pdf",      "application/pdf",
        "zip",      "application/zip",
        "xml",      "application/xml"
    };

    //--------------------------------------------------------------------------
    //
    // implementation of utility class Base64
    //
    //--------------------------------------------------------------------------
    const char* Base64::GetMimeType( const TextSpan& fileName ) {
        const char* pTop = fileName.Top();
        const char* pEnd = fileName.End();
        const char* pSuffix = nullptr;
        for( const char* p = pTop; p < pEnd; ++p ) {
            if( *p == '.' )
                pSuffix = p + 1;
        }
        if( !pSuffix )
            pSuffix = pEnd;
        TextSpan suffix{ pSuffix, pEnd };
        for( uint32_t i = 0; i < sizeof(s_MimeTypes)/sizeof(s_MimeTypes[0]); i += 2 ) {
            if( suffix.IsEqual( s_MimeTypes[i] ) == true )
                return s_MimeTypes[i+1];
        }
        return "application/octet-stream";
    }

    void Base64::EncodeFile( std::ostream& os, const TextSpan& fileName ) {
        auto pFile = File::LoadWhole( fileName );
        uint32_t length  = pFile->ByteLength();
        uint8_t* pBuffer = pFile->GetBuffer<uint8_t>();
        uint32_t count   = 0;
        while( 0 < length ) {
            os << Base64::Encode( pBuffer, length );
            if( ++count == 19 ) {
                os << std::endl;
                count = 0;
            }
        }
        os << std::endl;
        File::ReleaseWholeFile( pFile );
    }

}    // namespace turnup

