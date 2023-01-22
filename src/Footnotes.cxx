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
		uint32_t Register( const TextSpan& tag, const TextSpan& note );
		void WriteFootnotes( std::ostream& os, DocumentInfo& docInfo ) const;
		void WriteFootnotes( const TextSpan& tag, 
							 std::ostream& os, DocumentInfo& docInfo ) const;
	private:
		typedef std::vector<TextSpan>			NoteList;
		typedef std::pair<TextSpan, NoteList*>	TaggedNotes;
		typedef std::vector<TaggedNotes>		TaggedNoteList;
	private:
		NoteList* AddTaggedNoteList( const TextSpan& tag );
		NoteList* GetNoteListFromTag( const TextSpan& tag ) const;
	private:
		NoteList*		m_pNotes;
		TaggedNoteList*	m_pTaggedNoteList;
	};

	//--------------------------------------------------------------------------
	//
	// implementaion of class Footnotes
	//
	//--------------------------------------------------------------------------
	Footnotes::Footnotes() : m_pImpl( nullptr ) {
	}
	Footnotes::~Footnotes() {
		delete m_pImpl;
	}
	uint32_t Footnotes::Register( const char* pNote, const char* pNoteEnd ) {
		if( !m_pImpl )
			m_pImpl = new Impl{};
		return m_pImpl->Register( pNote, pNoteEnd );
	}
	uint32_t Footnotes::Register( const TextSpan& tag, const TextSpan& note ) {
		if( !m_pImpl )
			m_pImpl = new Impl{};
		return m_pImpl->Register( tag, note );
	}
	void Footnotes::WriteFootnotes( std::ostream& os, DocumentInfo& docInfo ) const {
		if( m_pImpl )
			m_pImpl->WriteFootnotes( os, docInfo );
	}
	void Footnotes::WriteFootnotes( const TextSpan& tag, 
									std::ostream& os, DocumentInfo& docInfo ) const {
		if( m_pImpl )
	 		m_pImpl->WriteFootnotes( tag, os, docInfo );
	}

	//--------------------------------------------------------------------------
	//
	// implementaion of class Footnotes::Impl
	//
	//--------------------------------------------------------------------------
	Footnotes::Impl::Impl() : m_pNotes( nullptr ),
							  m_pTaggedNoteList( nullptr ) {
	}

	Footnotes::Impl::~Impl() {
		if( m_pNotes )
			delete m_pNotes;
		if( m_pTaggedNoteList ) {
			for( TaggedNotes& entry : *m_pTaggedNoteList ) {
				delete entry.second;
				entry.second = nullptr;
			}
			delete m_pTaggedNoteList;
		}
	}

	uint32_t Footnotes::Impl::Register( const char* pNote, const char* pNoteEnd ) {
		if( !m_pNotes )
			m_pNotes = new NoteList{};
		m_pNotes->emplace_back( pNote, pNoteEnd );
		return m_pNotes->size();
	}

	uint32_t Footnotes::Impl::Register( const TextSpan& tag, const TextSpan& note ) {
		NoteList* pNotes = GetNoteListFromTag( tag );
		if( !pNotes )
			pNotes = AddTaggedNoteList( tag );
		pNotes->emplace_back( note.Top(), note.End() );
		return pNotes->size();
	}

	void Footnotes::Impl::WriteFootnotes( std::ostream& os,
										  DocumentInfo& docInfo ) const {
		auto& styles = docInfo.Get<StyleStack>();
		auto itr1 = m_pNotes->begin();
		auto itr2 = m_pNotes->end();
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

	void Footnotes::Impl::WriteFootnotes( const TextSpan& tag, 
										  std::ostream& os,
										  DocumentInfo& docInfo ) const {
		NoteList* pNotes = GetNoteListFromTag( tag );
		if( pNotes ) {
			auto& styles = docInfo.Get<StyleStack>();
			auto itr1 = pNotes->begin();
			auto itr2 = pNotes->end();
			for( uint32_t idx = 0; itr1 != itr2; ++itr1 ) {
				++idx;
				const TextSpan& line = *itr1;
				styles.WriteOpenTag( os, "p", " class='footnote'" );
				os << "<a name='footnote_"      << tag << idx << "'"
				   <<   " href='#footnote_ref_" << tag << idx << "'>" << idx << "</a> : ";
				line.WriteTo( os, docInfo );
				os << "</p>" << std::endl;
			}
		}			
	}

	Footnotes::Impl::NoteList* Footnotes::Impl::AddTaggedNoteList( const TextSpan& tag ) {
		if( !m_pTaggedNoteList )
			m_pTaggedNoteList = new TaggedNoteList{};
		NoteList* pNew = new NoteList{};
		m_pTaggedNoteList->emplace_back( tag, pNew );
		return pNew;
	}

	Footnotes::Impl::NoteList* Footnotes::Impl::GetNoteListFromTag( const TextSpan& tag ) const {
		if( m_pTaggedNoteList && (0 < m_pTaggedNoteList->size()) ) {
			// 多くの場合で最後のがヒットするはずなので先にチェック
			TaggedNotes& last = m_pTaggedNoteList->back();
			if( tag.IsEqual( last.first ) == true )
				return last.second;
			// 上記がダメなら既存エントリを全てチェック
			for( TaggedNotes& entry : *m_pTaggedNoteList ) {
				if( tag.IsEqual( entry.first ) == true )
					return last.second;
			}
		}
		return nullptr;
	}

} // namespace turnup

