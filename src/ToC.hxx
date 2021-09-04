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

	class DocumentInfo;
	class Config;
	class TextSpan;

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
		bool RegisterHeader( uint32_t level, const TextSpan& title );
		bool RegisterTable( const TextSpan& title );
		bool RegisterFigure( const TextSpan& title );
		const char* GetAnchorTag( EntryT type,
								  const char* pTitle,
								  const char* pTitleEnd = nullptr ) const;
		bool GetEntryNumber( char* pBuf, EntryT type, const Config& cfg,
							 const char* pTitle, const char* pTitleEnd = nullptr ) const;
	public:
		void WriteTOC( std::ostream& os,
					   DocumentInfo& docInfo,
					   uint32_t minLevel, uint32_t maxLevel ) const;
		void WriteTableList( std::ostream& os, DocumentInfo& docInfo ) const;
		void WriteFigureList( std::ostream& os, DocumentInfo& docInfo ) const;
	private:
		class Impl;
		Impl* m_pImpl;
	};

} // namespace turnup

#endif // TOC_HXX__
