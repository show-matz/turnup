//------------------------------------------------------------------------------
//
// ToC.hxx
//
//------------------------------------------------------------------------------
#ifndef TOC_HXX__
#define TOC_HXX__

#include <stdint.h>
#include <iosfwd>

namespace turnup {

	class Config;

	//--------------------------------------------------------------------------
	//
	// class ToC
	//
	//--------------------------------------------------------------------------
	class ToC {
	public:
		ToC();
		~ToC();
	public:
		void Register( uint32_t level, const char* pTitle );
		const char* GetAnchorTag( const char* pTitle,
								  const char* pTitleEnd = nullptr ) const;
		bool GetChapterNumber( char* pBuf, const Config& cfg,
							   const char* pTitle, const char* pTitleEnd = nullptr ) const;
	public:
		void WriteTOC( std::ostream& os, const Config& cfg,
					   uint32_t minLevel, uint32_t maxLevel ) const;
	private:
		class Impl;
		Impl* m_pImpl;
	};

} // namespace turnup

#endif // TOC_HXX__
