//------------------------------------------------------------------------------
//
// FilterBuffer.hxx
//
//------------------------------------------------------------------------------
#ifndef FILTERBUFFER_HXX__
#define FILTERBUFFER_HXX__

#include "TextSpan.hxx"

#include <stdint.h>

namespace turnup {

	class TextSpan;

	//--------------------------------------------------------------------------
	//
	// class FilterBuffer
	//
	//--------------------------------------------------------------------------
	class FilterBuffer {
	public:
		FilterBuffer( const TextSpan* pTop, const TextSpan* pEnd );
		~FilterBuffer();
	public:
		TextSpan GetBuffer() const;
	private:
		uint32_t	m_length;
		char*		m_pBuffer;
	};

} // namespace turnup

#endif // FILTERBUFFER_HXX__
