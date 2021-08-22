//------------------------------------------------------------------------------
//
// Footnotes.cxx
//
//------------------------------------------------------------------------------
#include "Footnotes.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"

#include <vector>
#include <ostream>

namespace turnup {

	class ToC;

	//--------------------------------------------------------------------------
	//
	// class Footnotes::Impl
	//
	//--------------------------------------------------------------------------
	class Footnotes::Impl {
	public:
		Impl();
		~Impl();
	public:
		uint32_t Register( const char* pNote, const char* pNoteEnd );
		void WriteFootnotes( std::ostream& os, DocumentInfo& docInfo ) const;
	private:
		std::vector<TextSpan> m_notes;
	};

	//--------------------------------------------------------------------------
	//
	// implementaion of class Footnotes
	//
	//--------------------------------------------------------------------------
	Footnotes::Footnotes() : m_pImpl( new Impl{} ) {
	}
	Footnotes::~Footnotes() {
		delete m_pImpl;
	}
	uint32_t Footnotes::Register( const char* pNote, const char* pNoteEnd ) {
		return m_pImpl->Register( pNote, pNoteEnd );
	}
	void Footnotes::WriteFootnotes( std::ostream& os, DocumentInfo& docInfo ) {
 		m_pImpl->WriteFootnotes( os, docInfo );
	}

	//--------------------------------------------------------------------------
	//
	// implementaion of class Footnotes::Impl
	//
	//--------------------------------------------------------------------------
	Footnotes::Impl::Impl() : m_notes() {
	}

	Footnotes::Impl::~Impl() {
		m_notes.clear();
	}

	uint32_t Footnotes::Impl::Register( const char* pNote, const char* pNoteEnd ) {
		m_notes.emplace_back( pNote, pNoteEnd );
		return m_notes.size();
	}

	void Footnotes::Impl::WriteFootnotes( std::ostream& os,
										  DocumentInfo& docInfo ) const {
		auto& styles = docInfo.Get<StyleStack>();
		auto itr1 = m_notes.begin();
		auto itr2 = m_notes.end();
		for( uint32_t idx = 0; itr1 != itr2; ++itr1 ) {
			++idx;
			const TextSpan& line = *itr1;
			styles.WriteOpenTag( os, "p", " class='footnote'" );
			os << "<a name='footnote" << idx << "' href='#footnote_ref" << idx << "'>"
			   << idx << "</a> : ";
			line.WriteTo( os, docInfo );
			os << "</p>" << std::endl;
		}
	}


} // namespace turnup

