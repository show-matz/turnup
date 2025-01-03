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
#include <algorithm>

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
        bool RegisterPrefix( const TextSpan& tag, const TextSpan& prefix );
        void GetPrefix( const TextSpan& tag, TextSpan& prefix ) const;
        void WriteFootnotes( std::ostream& os, DocumentInfo& docInfo ) const;
        void WriteFootnotes( const TextSpan& tag, 
                             std::ostream& os, DocumentInfo& docInfo ) const;
    private:
        typedef std::vector<TextSpan>           NoteList;
        typedef std::pair<TextSpan, NoteList*>  TaggedNotes;
        typedef std::pair<TextSpan, TextSpan>   PrefixInfo;
        typedef std::vector<TaggedNotes>        TaggedNoteList;
        typedef std::vector<PrefixInfo>         PrefixList;
    private:
        NoteList* AddTaggedNoteList( const TextSpan& tag );
        NoteList* GetNoteListFromTag( const TextSpan& tag ) const;
    private:
        NoteList*        m_pNotes;
        TaggedNoteList*  m_pTaggedNoteList;
        PrefixList*      m_pPrefixList;
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
    bool Footnotes::RegisterPrefix( const TextSpan& tag, const TextSpan& prefix ) {
        if( !m_pImpl )
            m_pImpl = new Impl{};
        return m_pImpl->RegisterPrefix( tag, prefix );
    }
    void Footnotes::GetPrefix( const TextSpan& tag, TextSpan& prefix ) const {
        if( !!m_pImpl )
            return m_pImpl->GetPrefix( tag, prefix );
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
                              m_pTaggedNoteList( nullptr ),
                              m_pPrefixList( nullptr ) {
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
        if( m_pPrefixList )
            delete m_pPrefixList;
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

    bool Footnotes::Impl::RegisterPrefix( const TextSpan& tag, const TextSpan& prefix ) {
        if( !m_pPrefixList )
            m_pPrefixList = new PrefixList{};
        auto itr = std::find_if( m_pPrefixList->begin(), m_pPrefixList->end(),
                                 [&tag]( const PrefixInfo& info ) -> bool {
                                     return tag.IsEqual( info.first );
                                 } );
        if( itr != m_pPrefixList->end() )
            return false;
        m_pPrefixList->emplace_back( tag, prefix );
        return true;
    }

    void Footnotes::Impl::GetPrefix( const TextSpan& tag, TextSpan& prefix ) const {
        if( !m_pPrefixList ) {
            prefix.Clear();
            return;
        }
        auto itr = std::find_if( m_pPrefixList->begin(), m_pPrefixList->end(),
                                 [&tag]( const PrefixInfo& info ) -> bool {
                                     return tag.IsEqual( info.first );
                                 } );
        if( itr != m_pPrefixList->end() )
            prefix = itr->second;
        else
            prefix.Clear();
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
        TextSpan prefix{};
        this->GetPrefix( tag, prefix );

        NoteList* pNotes = GetNoteListFromTag( tag );
        if( pNotes ) {
            auto& styles = docInfo.Get<StyleStack>();
            auto itr1 = pNotes->begin();
            auto itr2 = pNotes->end();
            for( uint32_t idx = 0; itr1 != itr2; ++itr1 ) {
                ++idx;
                const TextSpan& line = *itr1;
                styles.WriteOpenTag( os, "p", " class='footnote'" );
                os << "<a name='footnote_"      << tag << "_" << idx << "'"
                   <<   " href='#footnote_ref_" << tag << "_" << idx << "'>"
                   << prefix << idx << "</a> : ";
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
            /* 多くの場合で最後のがヒットするはずなので先にチェック */ {
                TaggedNotes& last = m_pTaggedNoteList->back();
                if( tag.IsEqual( last.first ) == true )
                    return last.second;
            }
            // 上記がダメなら既存エントリを全てチェック
            for( TaggedNotes& entry : *m_pTaggedNoteList ) {
                if( tag.IsEqual( entry.first ) == true )
                    return entry.second;
            }
        }
        return nullptr;
    }

} // namespace turnup

