//------------------------------------------------------------------------------
//
// File.hxx
//
//------------------------------------------------------------------------------
#ifndef FILE_HXX__
#define FILE_HXX__

#include <cstdint>

namespace turnup {

    class TextSpan;

    //--------------------------------------------------------------------------
    //
    // class WholeFile
    //
    //--------------------------------------------------------------------------
    class WholeFile {
    public:
        WholeFile();
        WholeFile( const WholeFile& ) = delete;
        WholeFile& operator=( const WholeFile& ) = delete;
        virtual ~WholeFile();
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

    //--------------------------------------------------------------------------
    //
    // utility class File
    //
    //--------------------------------------------------------------------------
    class File {
    public:
        File() = delete;
        File( const File& ) = delete;
        File& operator=( const File& ) = delete;
        ~File() = delete;
    public:
        static bool IsFullPath( const TextSpan& fileName );
        static TextSpan GetPath( const TextSpan& filePath );
        static bool IsExist( const TextSpan& fileName );
        static bool IsExist( const TextSpan& basePath,
                             const TextSpan& relPath, TextSpan* pResult );
        static bool Remove( const char* pFileName );
        static WholeFile* LoadWhole( const char* pFileName );
        static WholeFile* LoadWhole( const TextSpan& fileName );
        static void ReleaseWholeFile( WholeFile* pWholeFile );
    };

}    // namespace turnup

#endif // FILE_HXX__
