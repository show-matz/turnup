//------------------------------------------------------------------------------
//
// Buffer.hxx
//
//------------------------------------------------------------------------------
#ifndef BUFFER_HXX__
#define BUFFER_HXX__

#include "TextSpan.hxx"

namespace turnup {

	class TextSpan;

	//--------------------------------------------------------------------------
	//
	// class Buffer
	//
	//--------------------------------------------------------------------------
	class Buffer {
		friend Buffer& operator<<( Buffer& buf, const char* p );
		friend Buffer& operator<<( Buffer& buf, const TextSpan& span );
	public:
		class Param;
	public:
		Buffer();
		~Buffer();
	public:
		TextSpan GetSpan() const;
	private:
		Param* m_pLast;
	};

	Buffer& operator<<( Buffer& buf, const char* p );
	Buffer& operator<<( Buffer& buf, const TextSpan& span );


} // namespace turnup

#endif // BUFFER_HXX__
