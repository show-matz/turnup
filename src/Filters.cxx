//------------------------------------------------------------------------------
//
// Filters.hxx
//
//------------------------------------------------------------------------------
#include "Filters.hxx"

#include "TextSpan.hxx"

//-------------------
#include "CRC64.hxx"
#include "File.hxx"
#include <stdlib.h>
#include <fstream>
//-------------------

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string.h>

namespace turnup {

	static void DefaultFilter( std::ostream& os,
							   const TextSpan* pTop, const TextSpan* pEnd );
	static bool ExternalFilter( std::ostream& os, const TextSpan& command,
								const TextSpan* pTop, const TextSpan* pEnd );

	//--------------------------------------------------------------------------
	//
	// class Filters::Impl
	//
	//--------------------------------------------------------------------------
	class Filters::Impl {
	private:
		typedef std::pair<TextSpan,TextSpan>	Filter;
		typedef std::vector<Filter>				FilterList;
	public:
		Impl();
		~Impl();
	public:
		void RegistFilter( const TextSpan& label, const TextSpan& command );
		bool ExecuteFilter( std::ostream& os, const TextSpan& type,
							const TextSpan* pTop, const TextSpan* pEnd );
	private:
		FilterList m_filterList;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class Filters
	//
	//--------------------------------------------------------------------------
	Filters::Filters() : m_pImpl( new Impl{} ) {
	}
	Filters::~Filters() {
		delete m_pImpl;
	}
	void Filters::RegistFilter( const TextSpan& label, const TextSpan& command ) {
		return m_pImpl->RegistFilter( label, command );
	}
	bool Filters::ExecuteFilter( std::ostream& os, const TextSpan& type,
								 const TextSpan* pTop, const TextSpan* pEnd ) {
		return m_pImpl->ExecuteFilter( os, type, pTop, pEnd );
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class Filters::Impl
	//
	//--------------------------------------------------------------------------
	Filters::Impl::Impl() : m_filterList( {} ) {
	}
	Filters::Impl::~Impl() {
		m_filterList.clear();
	}
	void Filters::Impl::RegistFilter( const TextSpan& label, const TextSpan& command ) {
		m_filterList.emplace_back( label, command );
	}
	bool Filters::Impl::ExecuteFilter( std::ostream& os, const TextSpan& type,
									   const TextSpan* pTop, const TextSpan* pEnd ) {
		if( type.IsEmpty() ) {
			DefaultFilter( os, pTop, pEnd );
			return true;
		}
		auto itr = std::find_if( m_filterList.begin(), m_filterList.end(),
								 [&type]( const Filter& filter ) -> bool {
									 return filter.first.IsEqual( type );
								 } );
		if( itr != m_filterList.end() )
			return ExternalFilter( os, itr->second, pTop, pEnd );

		DefaultFilter( os, pTop, pEnd );
		return false;
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static void DefaultFilter( std::ostream& os,
							   const TextSpan* pTop, const TextSpan* pEnd ) {
		os << "<pre>" << std::endl;
		for( ; pTop < pEnd; ++pTop ) {
			pTop->WriteSimple( os ) << std::endl;
		}
		os << "</pre>" << std::endl;
	}

	static bool ExternalFilter( std::ostream& os, const TextSpan& command,
								const TextSpan* pTop, const TextSpan* pEnd ) {
		char inFile[16];
		char outFile[16];
		CRC64::Calc( pTop->Top(), pEnd->Top(), inFile );
		::strcpy( outFile, inFile );
		::strcat( inFile,  ".in" );
		::strcat( outFile, ".out" );
		std::ofstream ofs{ inFile };
		for( ; pTop < pEnd; ++pTop ) {
			pTop->WriteSimple( ofs );
			ofs << std::endl;
		}
		const char* pCmdTop = command.Top();
		const char* pCmdEnd = command.End();
		const char* inTag  = "%in";
		const char* outTag = "%out";
		const char* pIn  = std::search( pCmdTop, pCmdEnd,  inTag,  inTag + 3 );
		const char* pOut = std::search( pCmdTop, pCmdEnd, outTag, outTag + 4 );
		if( pIn == pCmdEnd || pOut == pCmdEnd )
			return false;

		char cmdBuf[1024];
		char* pBuf = cmdBuf;
		if( pIn < pOut ) {
			pBuf = std::copy( pCmdTop, pIn, pBuf );
			::strcpy( pBuf, inFile );
			pBuf   += ::strlen( pBuf );
			pCmdTop = pIn + 3;
			pBuf = std::copy( pCmdTop, pOut, pBuf );
			::strcpy( pBuf, outFile );
			pBuf   += ::strlen( pBuf );
			pCmdTop = pOut + 4;
			pBuf = std::copy( pCmdTop, pCmdEnd, pBuf );
			*pBuf = 0;
		} else {
			pBuf = std::copy( pCmdTop, pOut, pBuf );
			::strcpy( pBuf, outFile );
			pBuf   += ::strlen( pBuf );
			pCmdTop = pOut + 4;
			pBuf = std::copy( pCmdTop, pIn, pBuf );
			::strcpy( pBuf, inFile );
			pBuf   += ::strlen( pBuf );
			pCmdTop = pIn + 3;
			pBuf = std::copy( pCmdTop, pCmdEnd, pBuf );
			*pBuf = 0;
		}
		::system( cmdBuf );

		WholeFile* pSVG = File::LoadWhole( outFile );
		os << pSVG->GetBuffer<char>() << std::endl;
		File::ReleaseWholeFile( pSVG );

		// remove temporary file
		File::Remove( inFile );
		File::Remove( outFile );

		return true;
	}

} // namespace turnup

