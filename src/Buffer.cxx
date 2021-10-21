//------------------------------------------------------------------------------
//
// Buffer.cxx
//
//------------------------------------------------------------------------------
#include "Buffer.hxx"

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
	// class Buffer::Param
	//
	//--------------------------------------------------------------------------
	class Buffer::Param {
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
	// class BufferCleaner
	//
	//--------------------------------------------------------------------------
	class BufferCleaner {
	public:
		BufferCleaner() {}
		~BufferCleaner() {
			for( char*& pBuf : s_buffers ) {
			#ifndef NDEBUG
				std::cerr << "NOTE : release buffer 0x"
						  << std::hex << (void*)pBuf  << std::endl;
			#endif //NDEBUG
				delete[] pBuf;
				pBuf = nullptr;
			}
			s_buffers.clear();
			Buffer::Param::Cleanup();
		};
	} s_bufCleaner;

	//--------------------------------------------------------------------------
	//
	// implementation class Buffer
	//
	//--------------------------------------------------------------------------
	Buffer::Buffer() : m_pLast( nullptr ) {
	}
	Buffer::~Buffer() {
		while( m_pLast != nullptr ) {
			auto p = m_pLast;
			m_pLast = p->GetPrev();
			Param::Release( p );
		}
	}
	TextSpan Buffer::GetSpan() const {
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
	Buffer& operator<<( Buffer& buf, const char* p ) {
		Buffer::Param* pParam = Buffer::Param::Create( TextSpan{ p } );
		pParam->SetPrev( buf.m_pLast );
		buf.m_pLast = pParam;
		return buf;
	}
	Buffer& operator<<( Buffer& buf, const TextSpan& span ) {
		Buffer::Param* pParam = Buffer::Param::Create( span );
		pParam->SetPrev( buf.m_pLast );
		buf.m_pLast = pParam;
		return buf;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class Buffer::Param
	//
	//--------------------------------------------------------------------------
	Buffer::Param* Buffer::Param::s_pCemetery = nullptr;

	Buffer::Param::Param( const TextSpan& ts ) : m_data( ts ),
												 m_pPrev( nullptr ) {
	}
	Buffer::Param::~Param() {
	}
	uint32_t Buffer::Param::GetTotalLength() const {
		return m_data.ByteLength() + (!m_pPrev ? 0 : m_pPrev->GetTotalLength());
	}
	char* Buffer::Param::WriteToBuffer( char* pBuffer ) const {
		if( m_pPrev )
			pBuffer = m_pPrev->WriteToBuffer( pBuffer );
		return std::copy( m_data.Top(), m_data.End(), pBuffer );
	}
	Buffer::Param* Buffer::Param::Create( const TextSpan& ts ) {
		Param* pParam = s_pCemetery;
		if( !pParam ) {
			pParam = new Param{ ts };
		#ifndef NDEBUG
			std::cerr << "NOTE : allocate Buffer::Param 0x"
					  << std::hex << (void*)pParam  << std::endl;
		#endif //NDEBUG
		} else {
		#ifndef NDEBUG
			std::cerr << "NOTE : recycle Buffer::Param 0x"
					  << std::hex << (void*)pParam  << std::endl;
		#endif //NDEBUG
			s_pCemetery    = pParam->m_pPrev;
			pParam->m_data  = ts;
			pParam->m_pPrev = nullptr;
		}
		return pParam;
	}
	void Buffer::Param::Release( Param* p ) {
		p->m_data   = TextSpan{};
		p->m_pPrev  = s_pCemetery;
		s_pCemetery = p; 
	}
	void Buffer::Param::Cleanup() {
		while( s_pCemetery ) {
			auto p = s_pCemetery;
		#ifndef NDEBUG
			std::cerr << "NOTE : release Buffer::Param 0x"
					  << std::hex << (void*)p  << std::endl;
		#endif //NDEBUG
			s_pCemetery = p->GetPrev();
			delete p;
		}
	}

} // namespace turnup

