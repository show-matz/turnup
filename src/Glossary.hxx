//------------------------------------------------------------------------------
//
// Glossary.hxx
//
//------------------------------------------------------------------------------
#ifndef GLOSSARY_HXX__
#define GLOSSARY_HXX__

#include <iosfwd>

namespace turnup {

	typedef void WriteFunction( std::ostream&, const char*, const char* );

	//--------------------------------------------------------------------------
	//
	// class Glossary
	//
	//--------------------------------------------------------------------------
	class Glossary {
	public:
		Glossary();
		~Glossary();
	public:
		void Register( const char* pTop, const char* pEnd );
		const char* GetAnchorTag( const char* pTerm,
								  const char* pTermEnd = nullptr ) const;
		void WriteWithTermLink( std::ostream& os, const char* pTop,
								const char* pEnd, WriteFunction* pWriteFunc ) const;
	private:
		class Impl;
		Impl* m_pImpl;
	};

} // namespace turnup

#endif // GLOSSARY_HXX__
