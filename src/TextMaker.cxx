//------------------------------------------------------------------------------
//
// TextMaker.cxx
//
//------------------------------------------------------------------------------
#include "TextMaker.hxx"

#include "TextSpan.hxx"

#include <vector>
#include <algorithm>

#ifndef NDEBUG
	#include <iostream>
	#include <iomanip>
#endif //NDEBUG

namespace turnup {

	class TextSpan;

	static std::vector<char*>	s_buffers;

	//--------------------------------------------------------------------------
	//
	// class TextMaker::Param
	//
	//--------------------------------------------------------------------------
	class TextMaker::Param {
	public:
		Param( const TextSpan& ts );
		~Param();
	public:
		inline Param* GetPrev() const { return m_pPrev; }
		inline void SetPrev( Param* p ) { m_pPrev = p; }
	public:
		uint32_t GetTotalLength() const;
		char* WriteToBuffer( char* pBuffer ) const;
	private:
		TextSpan	m_data;
		Param*		m_pPrev;
	public:
		static Param* Create( const TextSpan& ts );
		static void Release( Param* p );
		static void Cleanup();
	private:
		static Param*	s_pCemetery;
	};

	//--------------------------------------------------------------------------
	//
	// class TextMakerCleaner
	//
	//--------------------------------------------------------------------------
	class TextMakerCleaner {
	public:
		TextMakerCleaner() {}
		~TextMakerCleaner() {
			for( char*& pBuf : s_buffers ) {
			#ifndef NDEBUG
				std::cerr << "NOTE : release buffer 0x"
						  << std::hex << (void*)pBuf  << std::endl;
			#endif //NDEBUG
				delete[] pBuf;
				pBuf = nullptr;
			}
			s_buffers.clear();
			TextMaker::Param::Cleanup();
		};
	} s_bufCleaner;

	//--------------------------------------------------------------------------
	//
	// implementation class TextMaker
	//
	//--------------------------------------------------------------------------
	TextMaker::TextMaker() : m_pLast( nullptr ) {
	}
	TextMaker::~TextMaker() {
		while( m_pLast != nullptr ) {
			auto p = m_pLast;
			m_pLast = p->GetPrev();
			Param::Release( p );
		}
	}
	TextSpan TextMaker::GetSpan() const {
		if( !m_pLast )
			return TextSpan{};
		uint32_t len = m_pLast->GetTotalLength();
		char* pBuffer = new char[len+1];
		s_buffers.push_back( pBuffer );
		char* pEnd = m_pLast->WriteToBuffer( pBuffer );
		*pEnd = 0;
		#ifndef NDEBUG
			std::cerr << "NOTE : allocate buffer 0x"
					  << std::hex << (void*)pBuffer
					  << " (" << std::dec << len << "bytes) : "
					  << '"' << pBuffer << '"' << std::endl;
		#endif //NDEBUG
		return TextSpan{ pBuffer, pEnd };
	}
	TextMaker& operator<<( TextMaker& buf, const char* p ) {
		TextMaker::Param* pParam = TextMaker::Param::Create( TextSpan{ p } );
		pParam->SetPrev( buf.m_pLast );
		buf.m_pLast = pParam;
		return buf;
	}
	TextMaker& operator<<( TextMaker& buf, const TextSpan& span ) {
		TextMaker::Param* pParam = TextMaker::Param::Create( span );
		pParam->SetPrev( buf.m_pLast );
		buf.m_pLast = pParam;
		return buf;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class TextMaker::Param
	//
	//--------------------------------------------------------------------------
	TextMaker::Param* TextMaker::Param::s_pCemetery = nullptr;

	TextMaker::Param::Param( const TextSpan& ts ) : m_data( ts ),
												 m_pPrev( nullptr ) {
	}
	TextMaker::Param::~Param() {
	}
	uint32_t TextMaker::Param::GetTotalLength() const {
		return m_data.ByteLength() + (!m_pPrev ? 0 : m_pPrev->GetTotalLength());
	}
	char* TextMaker::Param::WriteToBuffer( char* pBuffer ) const {
		if( m_pPrev )
			pBuffer = m_pPrev->WriteToBuffer( pBuffer );
		return std::copy( m_data.Top(), m_data.End(), pBuffer );
	}
	TextMaker::Param* TextMaker::Param::Create( const TextSpan& ts ) {
		Param* pParam = s_pCemetery;
		if( !pParam ) {
			pParam = new Param{ ts };
		#ifndef NDEBUG
			std::cerr << "NOTE : allocate TextMaker::Param 0x"
					  << std::hex << (void*)pParam  << std::endl;
		#endif //NDEBUG
		} else {
		#ifndef NDEBUG
			std::cerr << "NOTE : recycle TextMaker::Param 0x"
					  << std::hex << (void*)pParam  << std::endl;
		#endif //NDEBUG
			s_pCemetery    = pParam->m_pPrev;
			pParam->m_data  = ts;
			pParam->m_pPrev = nullptr;
		}
		return pParam;
	}
	void TextMaker::Param::Release( Param* p ) {
		p->m_data   = TextSpan{};
		p->m_pPrev  = s_pCemetery;
		s_pCemetery = p; 
	}
	void TextMaker::Param::Cleanup() {
		while( s_pCemetery ) {
			auto p = s_pCemetery;
		#ifndef NDEBUG
			std::cerr << "NOTE : release TextMaker::Param 0x"
					  << std::hex << (void*)p  << std::endl;
		#endif //NDEBUG
			s_pCemetery = p->GetPrev();
			delete p;
		}
	}

} // namespace turnup

