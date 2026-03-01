#include "Base64.hxx"

#include <stdio.h>
#include <memory>       // for new (nothrow)

using namespace std;

namespace turnup {

    class TextSpan;

    //--------------------------------------------------------------------------
    //
    // class WholeFile
    //
    //--------------------------------------------------------------------------
    class WholeFile {
    public:
        WholeFile() {};
        WholeFile( const WholeFile& ) = delete;
        WholeFile& operator=( const WholeFile& ) = delete;
        virtual ~WholeFile() {};
    public:
        virtual uint32_t ByteLength() = 0;
        virtual void* GetRawBuffer() = 0;
    public:
        template <typename T> inline T* GetBuffer() {
            return reinterpret_cast<T*>( this->GetRawBuffer() );
        }
        template <typename T> inline uint32_t Count() {
            return this->ByteLength() / sizeof(T);
        }
    };

    class WholeFileImpl : public WholeFile {
    public:
        WholeFileImpl( uint8_t* pBuf, uint32_t byteLength ) : m_pBuffer( pBuf ),
                                                              m_byteLength( byteLength ) {
        };
        ~WholeFileImpl() {
            if( m_pBuffer ) {
                delete[] m_pBuffer;
                m_pBuffer = nullptr;
                m_byteLength = 0;
            }
        };
    public:
        virtual uint32_t ByteLength() override {
            return this->m_byteLength;
        };
        virtual void* GetRawBuffer() override {
            return this->m_pBuffer;
        };
    private:
        uint8_t*    m_pBuffer;
        uint32_t    m_byteLength;
    };

    //--------------------------------------------------------------------------
    //
    // utility class File
    //
    //--------------------------------------------------------------------------
    class File {
    public:
            static WholeFile* LoadWhole( const char* pFileName ) {
    
            WholeFileImpl* pWhole = nullptr;
            FILE* pFile = ::fopen( pFileName, "rb" );
            if( !!pFile ) {
                do {
                    if( ::fseek( pFile, 0, SEEK_END ) != 0 )
                        break;
                    uint32_t fileSize = ::ftell( pFile );
                    if( ::fseek( pFile, 0, SEEK_SET ) != 0 )
                        break;
                    uint8_t* pBuf = new(nothrow) uint8_t[fileSize + 1];
                    if( !pBuf )
                        break;
                    if( 0 < fileSize ) {
                        if( ::fread( pBuf, fileSize, 1, pFile ) != 1 ) {
                            delete[] pBuf;
                            break;
                        }
                    }
                    pBuf[fileSize] = 0;
                    pWhole = new WholeFileImpl( pBuf, fileSize );
                } while( false );
                fclose( pFile );
            }
            return pWhole;
        }
        static void ReleaseWholeFile( WholeFile* pWholeFile ) {
            delete pWholeFile;
        };
    };

}    // namespace turnup

#include <iostream>

using namespace turnup;

int main(int argc, char* argv[]) {
    if( argc == 1 )
        return 1;
    auto pFile = File::LoadWhole( argv[1] );
    uint32_t length  = pFile->ByteLength();
    uint8_t* pBuffer = pFile->GetBuffer<uint8_t>();
    uint32_t count   = 0;
    while( 0 < length ) {
        std::cout << Base64::Encode( pBuffer, length );
        if( ++count == 19 ) {
            std::cout << std::endl;
            count = 0;
        }
    }
    std::cout << std::endl;
    File::ReleaseWholeFile( pFile );
    return 0;
}

