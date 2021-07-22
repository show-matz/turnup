//------------------------------------------------------------------------------
//
// File.cxx
//
//------------------------------------------------------------------------------
#include "File.hxx"

#include <sys/stat.h>	//ToDo : C標準ライブラリの範囲内で実現する必要がある。
#include <memory>		// for new (nothrow)
#include <stdio.h>

using namespace std;

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class WholeFile
	//
	//--------------------------------------------------------------------------
	WholeFile::WholeFile() {
	}
	WholeFile::~WholeFile() {
	}

	//--------------------------------------------------------------------------
	//
	// class WholeFileImpl
	//
	//--------------------------------------------------------------------------
	class WholeFileImpl : public WholeFile {
	public:
		WholeFileImpl( uint8_t* pBuf, uint32_t byteLength );
		~WholeFileImpl();
	public:
		virtual uint32_t ByteLength() override;
		virtual void* GetRawBuffer() override;
	private:
		uint8_t*	m_pBuffer;
		uint32_t	m_byteLength;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class WholeFileImpl
	//
	//--------------------------------------------------------------------------
	WholeFileImpl::WholeFileImpl( uint8_t* pBuf,
								  uint32_t byteLength ) : m_pBuffer( pBuf ),
														  m_byteLength( byteLength ) {
	}

	WholeFileImpl::~WholeFileImpl() {
		if( m_pBuffer ) {
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			m_byteLength = 0;
		}
	}

	uint32_t WholeFileImpl::ByteLength() {
		return this->m_byteLength;
	}

	void* WholeFileImpl::GetRawBuffer() {
		return this->m_pBuffer;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class File
	//
	//--------------------------------------------------------------------------
	bool File::IsExist( const char* pFileName ) {
		//ToDo : ここ、C標準ライブラリの範囲内で実現する必要がある。
		struct stat st;
		return !::stat( pFileName, &st );
	}

	bool File::Remove( const char* pFileName ) {
		//ToDo : ここ、C標準ライブラリの範囲内で実現する必要がある。
		return !::remove( pFileName );
	}

	WholeFile* File::LoadWhole( const char* pFileName ) {
		
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

	void File::ReleaseWholeFile( WholeFile* pWholeFile ) {
		delete pWholeFile;
	}

} // namespace turnup

