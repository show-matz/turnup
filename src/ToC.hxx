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
		enum class EntryT {
			HEADER = 0,
			TABLE  = 1,
			FIGURE = 2
		};
	public:
		ToC();
		~ToC();
	public:
		void RegisterHeader( uint32_t level, const char* pTitle );
		void RegisterTable( const char* pTitle );
		void RegisterFigure( const char* pTitle );
		const char* GetAnchorTag( EntryT type,
								  const char* pTitle,
								  const char* pTitleEnd = nullptr ) const;
		bool GetEntryNumber( char* pBuf, EntryT type, const Config& cfg,
							 const char* pTitle, const char* pTitleEnd = nullptr ) const;
	public:
		void WriteTOC( std::ostream& os, const Config& cfg,
					   uint32_t minLevel, uint32_t maxLevel ) const;
		void WriteTableList( std::ostream& os, const Config& cfg ) const;
		void WriteFigureList( std::ostream& os, const Config& cfg ) const;
	private:
		class Impl;
		Impl* m_pImpl;
	};

} // namespace turnup

#endif // TOC_HXX__
