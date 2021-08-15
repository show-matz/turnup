//------------------------------------------------------------------------------
//
// FilterBuffer.cxx
//
//------------------------------------------------------------------------------
#include "FilterBuffer.hxx"

#include <string.h>

namespace turnup {

	static uint32_t CalcLength( const TextSpan* pTop, const TextSpan* pEnd );

	//--------------------------------------------------------------------------
	//
	// implementation of class FilterBuffer
	//
	//--------------------------------------------------------------------------
	FilterBuffer::FilterBuffer( const TextSpan* pTop,
								const TextSpan* pEnd ) : m_length( CalcLength( pTop, pEnd ) ),
														 m_pBuffer( new char[m_length+1] ) {
		for( char* p = m_pBuffer; pTop < pEnd; ++pTop ) {
			uint32_t len = pTop->ByteLength();
			::strncpy( p, pTop->Top(), len );
			p += len;
			*p++ = 0x0A;
		}
	}

	FilterBuffer::~FilterBuffer() {
		delete[] m_pBuffer;
	}

	TextSpan FilterBuffer::GetBuffer() const {
		return TextSpan{ m_pBuffer, m_pBuffer + m_length }; 
	}

	//--------------------------------------------------------------------------
	//
	// implementation of local functions
	//
	//--------------------------------------------------------------------------
	static uint32_t CalcLength( const TextSpan* pTop, const TextSpan* pEnd ) {
		uint32_t total = 0;
		for( ; pTop < pEnd; ++pTop ) {
			total += pTop->ByteLength() + 1;
		}
		return total;
	}

} // namespace turnup

