//------------------------------------------------------------------------------
//
// Parameters.hxx
//
//------------------------------------------------------------------------------
#ifndef PARAMETERS_HXX__
#define PARAMETERS_HXX__

#include <stdint.h>

namespace turnup {

	class TextSpan;

	//--------------------------------------------------------------------------
	//
	// class Parameters
	//
	//--------------------------------------------------------------------------
	class Parameters {
	public:
		Parameters();
		~Parameters();
	public:
		bool Analyze( int argc, char* argv[] );
	public:
		bool GetTargetFile( TextSpan& ref ) const;
		bool VersionMode() const;	// --version
		uint32_t DefinitionCount() const;
		bool Definition( uint32_t idx, TextSpan& ref ) const;
//		uint32_t IncludePathCount() const;
//		bool IncludePath( uint32_t idx, TextSpan& ref ) const;
	private:
		class Impl;
		Impl*	m_pImpl;
	};


} // namespace turnup

#endif // PARAMETERS_HXX__
