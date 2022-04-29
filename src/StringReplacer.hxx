//------------------------------------------------------------------------------
//
// StringReplacer.hxx
//
//------------------------------------------------------------------------------
#ifndef STRINGREPLACER_HXX__
#define STRINGREPLACER_HXX__

#include "TextSpan.hxx"

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class StringReplacer
	//
	//--------------------------------------------------------------------------
	class StringReplacer {
	public:
		// replace %1~%9 to [pTop, pEnd)
		StringReplacer( const TextSpan* pTop, const TextSpan* pEnd );
		~StringReplacer();
	public:
		TextSpan operator()( const TextSpan& line );
	private:
		const TextSpan* const m_pTop;
		const TextSpan* const m_pEnd;
	};

} // namespace turnup

#endif // STRINGREPLACER_HXX__
