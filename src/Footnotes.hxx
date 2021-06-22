//------------------------------------------------------------------------------
//
// Footnotes.hxx
//
//------------------------------------------------------------------------------
#ifndef FOOTNOTES_HXX__
#define FOOTNOTES_HXX__

#include <stdint.h>
#include <iosfwd>

namespace turnup {

	class DocumentInfo;

	//--------------------------------------------------------------------------
	//
	// class Footnotes
	//
	//--------------------------------------------------------------------------
	class Footnotes {
	public:
		Footnotes();
		~Footnotes();
	public:
		uint32_t Register( const char* pNote, const char* pNoteEnd );
		public:
		void WriteFootnotes( std::ostream& os, DocumentInfo& docInfo );
	private:
		class Impl;
		Impl* m_pImpl;
	};

} // namespace turnup

#endif // FOOTNOTES_HXX__
