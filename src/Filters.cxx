//------------------------------------------------------------------------------
//
// Filters.hxx
//
//------------------------------------------------------------------------------
#include "Filters.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "InternalFilter.hxx"

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
							   const DocumentInfo& docInfo,
							   const TextSpan* pTop, const TextSpan* pEnd );
	static bool ExecExtFilter( std::ostream& os, const TextSpan& command,
							   const TextSpan* pTop, const TextSpan* pEnd );

	//--------------------------------------------------------------------------
	//
	// class Filters::Impl
	//
	//--------------------------------------------------------------------------
	class Filters::Impl {
	private:
		typedef std::pair<TextSpan,TextSpan>	ExtFilter;
		typedef std::vector<ExtFilter>			ExtFilterList;
	public:
		Impl();
		~Impl();
	public:
		void RegistExternal( const TextSpan& label, const TextSpan& command );
		bool ExecuteFilter( std::ostream& os, 
							DocumentInfo& docInfo,
							const TextSpan& type,
							const TextSpan* pTop, const TextSpan* pEnd );
	private:
		ExtFilterList m_externals;
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
	void Filters::RegistExternal( const TextSpan& label, const TextSpan& command ) {
		return m_pImpl->RegistExternal( label, command );
	}
	bool Filters::ExecuteFilter( std::ostream& os,
								 DocumentInfo& docInfo,
								 const TextSpan& type,
								 const TextSpan* pTop, const TextSpan* pEnd ) {
		return m_pImpl->ExecuteFilter( os, docInfo, type, pTop, pEnd );
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class Filters::Impl
	//
	//--------------------------------------------------------------------------
	Filters::Impl::Impl() : m_externals( {} ) {
	}
	Filters::Impl::~Impl() {
		m_externals.clear();
	}
	void Filters::Impl::RegistExternal( const TextSpan& label, const TextSpan& command ) {
		m_externals.emplace_back( label, command );
	}
	bool Filters::Impl::ExecuteFilter( std::ostream& os,
									   DocumentInfo& docInfo,
									   const TextSpan& type,
									   const TextSpan* pTop, const TextSpan* pEnd ) {
		// type 指定がなければデフォルトの <pre> 出力で終了
		if( type.IsEmpty() ) {
			DefaultFilter( os, docInfo, pTop, pEnd );
			return true;
		}
		/* 外部フィルタを優先して検索 */ {
			auto itr = std::find_if( m_externals.begin(), m_externals.end(),
									 [&type]( const ExtFilter& filter ) -> bool {
										 return filter.first.IsEqual( type );
									 } );
			// 該当する外部フィルタが見つかれば実行して終了
			if( itr != m_externals.end() )
				return ExecExtFilter( os, itr->second, pTop, pEnd );
		}
		/* 該当する外部フィルタがなければ次に内部フィルタを検索 */ {
			auto pFilter = InternalFilter::FindFilter( type );
			if( pFilter ) {
				// 該当する内部フィルタが見つかれば実行して終了
				return pFilter( os, docInfo, pTop, pEnd );
			}
		}
		// 指定された名前のフィルタが見つからない場合はデフォルトの <pre> 出力で false 復帰
		DefaultFilter( os, docInfo, pTop, pEnd );
		return false;
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static void DefaultFilter( std::ostream& os,
							   const DocumentInfo& docInfo,
							   const TextSpan* pTop, const TextSpan* pEnd ) {
		auto& styles = docInfo.Get<StyleStack>();
		styles.WriteOpenTag( std::cout, "pre" ) << std::endl;
		for( ; pTop < pEnd; ++pTop ) {
			pTop->WriteSimple( os ) << std::endl;
		}
		os << "</pre>" << std::endl;
	}

	static bool ExecExtFilter( std::ostream& os, const TextSpan& command,
							   const TextSpan* pTop, const TextSpan* pEnd ) {
		char inFile[16];
		char outFile[16];
		CRC64::Calc( 'X', pTop->Top(), pEnd->Top(), inFile ); // X means 'something other else'.
		::strcpy( outFile, inFile );
		::strcat( inFile,  ".in" );
		::strcat( outFile, ".out" );
		std::ofstream ofs{ inFile };
		for( ; pTop < pEnd; ++pTop ) {
			ofs.write( pTop->Top(), pTop->End() - pTop->Top() );
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

