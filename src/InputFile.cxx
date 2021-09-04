//------------------------------------------------------------------------------
//
// InputFile.cxx
//
//------------------------------------------------------------------------------
#include "InputFile.hxx"

#include "TextSpan.hxx"
#include "File.hxx"

#include <algorithm>
#include <vector>
#include <string.h>

namespace turnup {

	static char s_fileNameBuf[256+1];

	//--------------------------------------------------------------------------
	//
	// class InputFileImpl
	//
	//--------------------------------------------------------------------------
	class InputFileImpl : public InputFile {
	public:
		InputFileImpl( WholeFile* pFileData, const TextSpan& fileName );
		virtual ~InputFileImpl();
	public:
		virtual const TextSpan& GetFileName() const override;
		virtual uint32_t LineSize() const override;
		virtual const TextSpan* LineTop() const override;
		virtual const TextSpan* LineEnd() const override;
	private:
		static char* FixLineEnd( char* pTop, char* pEnd );
		static char* FixLineContinuous( char* pTop, char* pEnd );
	private:
		TextSpan				m_fileName;
		WholeFile*				m_pFileData;
		std::vector<TextSpan>	m_lines;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class InputFile
	//
	//--------------------------------------------------------------------------
	InputFile::InputFile() {
	}
	InputFile::~InputFile() {
	}
	InputFile* InputFile::LoadInputFile( const TextSpan& fileName ) {

		uint32_t nameLen = fileName.ByteLength();
		char* pFileName = nullptr;
		if( nameLen < sizeof(s_fileNameBuf) )
			pFileName = s_fileNameBuf;
		else
			pFileName = new char[nameLen+1];
		::strncpy( pFileName, fileName.Top(), nameLen );
		pFileName[nameLen] = 0;

		InputFile* pInputFile = nullptr;
		if( File::IsExist( pFileName ) ) {
			WholeFile* pFileData = File::LoadWhole( pFileName );
			pInputFile = new InputFileImpl{ pFileData, fileName };
		}
		if( pFileName != s_fileNameBuf )
			delete[] pFileName;
		return pInputFile;
	}

	void InputFile::ReleaseInputFile( InputFile* pInputFile ) {
		delete pInputFile;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class InputFileImpl
	//
	//--------------------------------------------------------------------------
	InputFileImpl::InputFileImpl( WholeFile* pFileData,
								  const TextSpan& fileName ) : InputFile(),
															   m_fileName( fileName ),
															   m_pFileData( pFileData ),
															   m_lines( ) {
		char* pTop = m_pFileData->GetBuffer<char>();
		char* pEnd = pTop + m_pFileData->Count<char>();
		pEnd = FixLineEnd( pTop, pEnd );
		pEnd = FixLineContinuous( pTop, pEnd );
		while( pTop < pEnd ) {
			char* pEOL = std::find( pTop, pEnd, 0x0A );
			char* pNext = pEOL + 1;
			pEOL[0] = 0;
			m_lines.emplace_back( pTop, pEOL );
			pTop = pNext;
		}
	}

	InputFileImpl::~InputFileImpl() {
		File::ReleaseWholeFile( m_pFileData );
	}

	const TextSpan& InputFileImpl::GetFileName() const {
		return m_fileName;
	}

	uint32_t InputFileImpl::LineSize() const {
		return m_lines.size();
	}

	const TextSpan* InputFileImpl::LineTop() const {
		if( m_lines.empty() )
			return nullptr;
		return &(m_lines[0]);
	}

	const TextSpan* InputFileImpl::LineEnd() const {
		if( m_lines.empty() )
			return nullptr;
		return &(m_lines[0]) + this->LineSize();
	}

	char* InputFileImpl::FixLineEnd( char* pTop, char* pEnd ) {
		char target[2] = { 0x0D, 0x0A };
		//ひとつめの CrLf を検索
		char* p = std::search( pTop, pEnd, target, target + 2 );
		if( p == pEnd ) {
			//みつからなければ何もしないでよし
			return pEnd;
		}
		pTop = p + 1;
		char* pDest = p;
		while( pTop < pEnd ) {
			p = std::search( pTop, pEnd, target, target + 2 );
			pDest = std::copy( pTop, p, pDest );
			pTop = p + 1;
		}
		*pDest = 0;
		return pDest;
	}

	char* InputFileImpl::FixLineContinuous( char* pTop, char* pEnd ) {
		char target[3] = { ' ', '\\', 0x0A };
		//ひとつめの継続行を検索
		char* p = std::search( pTop, pEnd, target, target + 3 );
		if( p == pEnd ) {
			//みつからなければ何もしないでよし
			return pEnd;
		}
		pTop = p + 3;
		char* pDest = p;
		while( pTop < pEnd ) {
			p = std::search( pTop, pEnd, target, target + 3 );
			pDest = std::copy( pTop, p, pDest );
			pTop = p + 3;
		}
		*pDest = 0;
		return pDest;
	}

} // namespace turnup

