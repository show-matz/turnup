//------------------------------------------------------------------------------
//
// Glossary.cxx
//
//------------------------------------------------------------------------------
#include "Glossary.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "CRC64.hxx"

#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class EntryBase
    //
    //--------------------------------------------------------------------------
    class EntryBase {
    public:
        EntryBase( const char* pTop, const char* pEnd );
        virtual ~EntryBase();
    public:
        virtual uint64_t GetHash() const = 0;
        virtual const char* GetAnchorTag() const = 0;
        virtual void Write( std::ostream& os,
                            WriteFunction* pWriteFunc ) const = 0;
    public:
        inline uint32_t GetLength() const { return m_length; }
        inline const char* GetTerm() const { return m_pTerm; }
        inline void Rebind( const char* pTop, const char* pEnd ) {
            m_pTerm  = pTop;
            m_length = pEnd - pTop;
        }
    private:
        const char* m_pTerm;    // 用語文字列のポインタ
        uint32_t    m_length;   // 用語文字列の長さ（バイト数）
    };

    EntryBase::EntryBase( const char* pTop, const char* pEnd ) : m_pTerm( pTop ),
                                                                 m_length( pEnd - pTop ) {
    }
    EntryBase::~EntryBase() {
    }

    //--------------------------------------------------------------------------
    //
    // class GlossaryEntry
    //
    //--------------------------------------------------------------------------
    class GlossaryEntry : public EntryBase {
    public:
        GlossaryEntry( const char* pTop, const char* pEnd );
        virtual ~GlossaryEntry();
    public:
        virtual uint64_t GetHash() const;
        virtual const char* GetAnchorTag() const;
        virtual void Write( std::ostream& os, WriteFunction* pWriteFunc ) const;
    private:
        uint64_t  m_hash;            // 用語文字列から生成されたハッシュ値
        char      m_anchorTag[12];   // ハッシュ値の文字列表現（null 終端を含む）
    };

    GlossaryEntry::GlossaryEntry( const char* pTop,
                                  const char* pEnd ) : EntryBase( pTop, pEnd ),
                                                       m_hash( 0 ) {
        m_hash = CRC64::Calc( 'G', pTop, pEnd, m_anchorTag ); // 'G' means glossary.
    }
    GlossaryEntry::~GlossaryEntry() {
        //intentionally do nothing.
    }
    uint64_t GlossaryEntry::GetHash() const {
        return m_hash;
    }
    const char* GlossaryEntry::GetAnchorTag() const {
        return m_anchorTag;
    }
    void GlossaryEntry::Write( std::ostream& os, WriteFunction* pWriteFunc ) const {
        os << "<a class='term' href='#" << this->GetAnchorTag()  << "'>";
        const char* p = this->GetTerm();
        pWriteFunc( os, p, p + this->GetLength() );
        os << "</a>";
    }

    //--------------------------------------------------------------------------
    //
    // class InternalAutoLinkEntry
    //
    //--------------------------------------------------------------------------
    class InternalAutoLinkEntry : public EntryBase {
    public:
        InternalAutoLinkEntry( const char* pTop, const char* pEnd,
                               const char* pTargetTop, const char* pTargetEnd );
        virtual ~InternalAutoLinkEntry();
    public:
        virtual uint64_t GetHash() const;
        virtual const char* GetAnchorTag() const;
        virtual void Write( std::ostream& os, WriteFunction* pWriteFunc ) const;
    private:
        uint64_t  m_hash;             // 生成されたハッシュ値
        char      m_anchorTag[12];    // ハッシュ値の文字列表現（null 終端を含む）
    };

    InternalAutoLinkEntry::InternalAutoLinkEntry( const char* pTop,
                                                  const char* pEnd,
                                                  const char* pTargetTop,
                                                  const char* pTargetEnd ) : EntryBase( pTop, pEnd ),
                                                                             m_hash( 0 ) {
        char type = 'H';
        const char* pTarget = nullptr;
        if( *pTargetTop == '#' )
            pTarget = pTargetTop + 1;
        else {
            type = *pTargetTop;
            pTarget = pTargetTop + 2;
        }
        m_hash = CRC64::Calc( type, pTarget, pTargetEnd, m_anchorTag );
        if( pEnd - pTop == 2 && pTop[0] == '$' && pTop[1] == '$' )
            this->Rebind( pTarget, pTargetEnd );
    }
    InternalAutoLinkEntry::~InternalAutoLinkEntry() {
        //intentionally do nothing.
    }
    uint64_t InternalAutoLinkEntry::GetHash() const {
        return m_hash;
    }
    const char* InternalAutoLinkEntry::GetAnchorTag() const {
        return m_anchorTag;
    }
    void InternalAutoLinkEntry::Write( std::ostream& os, WriteFunction* pWriteFunc ) const {
        os << "<a class='autolink' href='#" << this->GetAnchorTag()  << "'>";
        const char* p = this->GetTerm();
        pWriteFunc( os, p, p + this->GetLength() );
        os << "</a>";
    }

    //--------------------------------------------------------------------------
    //
    // class ExternalAutoLinkEntry
    //
    //--------------------------------------------------------------------------
    class ExternalAutoLinkEntry : public EntryBase {
    public:
        ExternalAutoLinkEntry( const char* pTop, const char* pEnd,
                               const char* pUrlTop, const char* pUrlEnd );
        virtual ~ExternalAutoLinkEntry();
    public:
        virtual uint64_t GetHash() const;
        virtual const char* GetAnchorTag() const;
        virtual void Write( std::ostream& os, WriteFunction* pWriteFunc ) const;
    private:
        const char* m_pUrlTop;
        const char* m_pUrlEnd;
    };

    ExternalAutoLinkEntry::ExternalAutoLinkEntry( const char* pTop,
                                                  const char* pEnd,
                                                  const char* pUrlTop,
                                                  const char* pUrlEnd ) : EntryBase( pTop, pEnd ),
                                                                          m_pUrlTop( pUrlTop ),
                                                                          m_pUrlEnd( pUrlEnd ) {
    }
    ExternalAutoLinkEntry::~ExternalAutoLinkEntry() {
        //intentionally do nothing.
    }
    uint64_t ExternalAutoLinkEntry::GetHash() const {
        return 0;
    }
    const char* ExternalAutoLinkEntry::GetAnchorTag() const {
        return nullptr;
    }
    void ExternalAutoLinkEntry::Write( std::ostream& os, WriteFunction* pWriteFunc ) const {
        os << "<a class='autolink' href='";
        os.write( m_pUrlTop, m_pUrlEnd - m_pUrlTop );
        os << "'>";
        const char* p = this->GetTerm();
        pWriteFunc( os, p, p + this->GetLength() );
        os << "</a>";
    }

    //--------------------------------------------------------------------------
    //
    // class Glossary::Impl
    //
    //--------------------------------------------------------------------------
    class Glossary::Impl {
    private:
        typedef std::pair<char, const EntryBase*> IndexEntry;
    public:
        Impl();
        ~Impl();
    public:
        bool RegisterTerm( const char* pTop, const char* pEnd );
        bool RegisterAutoLink( const char* pTop, const char* pEnd,
                               const char* pTargetTop, const char* pTargetEnd );
        const char* GetAnchorTag( const char* pTerm,
                                  const char* pTermEnd ) const;
        void SortIfNeed();
        void WriteWithTermLink( std::ostream& os,
                                const char* pTop, const char* pEnd,
                                uint32_t idx, WriteFunction* pWriteFunc ) const;
        void WriteIndex( std::ostream& os, DocumentInfo& docInfo ) const;
        void WriteIndex_X( std::ostream& os, DocumentInfo& docInfo ) const;
    private:
        bool CheckUniqueness( const char* pTop, const char* pEnd ) const;
        void SetupIndexEntries( std::vector<IndexEntry>& container ) const;
    private:
        static IndexEntry CreateIndexEntry( const EntryBase* pEntry );
        static bool CompareIndexEntry( const IndexEntry& e1, const IndexEntry& e2 );
        static const char* GetIndexTag( char mark );
    private:
        std::vector<EntryBase*> m_entries;
        bool m_sorted;
    };

    //--------------------------------------------------------------------------
    //
    // implementation of class Glossary
    //
    //--------------------------------------------------------------------------
    Glossary::Glossary() : m_pImpl( new Impl{} ) {
    }
    Glossary::~Glossary() {
        delete m_pImpl;
    }
    bool Glossary::RegisterTerm( const char* pTop, const char* pEnd ) {
        return m_pImpl->RegisterTerm( pTop, pEnd );
    }
    bool Glossary::RegisterAutoLink( const char* pTop, const char* pEnd,
                                     const char* pUrlTop, const char* pUrlEnd ) {
        return m_pImpl->RegisterAutoLink( pTop, pEnd, pUrlTop, pUrlEnd );
    }
    const char* Glossary::GetAnchorTag( const char* pTerm,
                                        const char* pTermEnd ) const {
        return m_pImpl->GetAnchorTag( pTerm, pTermEnd );
    }
    void Glossary::WriteWithTermLink( std::ostream& os,
                                      const char* pTop,
                                      const char* pEnd,
                                      WriteFunction* pWriteFunc ) const {
        m_pImpl->SortIfNeed();
        m_pImpl->WriteWithTermLink( os, pTop, pEnd, 0, pWriteFunc );
    }
    void Glossary::WriteIndex( std::ostream& os,
                               DocumentInfo& docInfo, bool bFoldable ) const {
        if( !bFoldable )
            m_pImpl->WriteIndex( os, docInfo );
        else
            m_pImpl->WriteIndex_X( os, docInfo );
    }

    //--------------------------------------------------------------------------
    //
    // implementation of class Glossary::Impl
    //
    //--------------------------------------------------------------------------
    Glossary::Impl::Impl() : m_entries(),
                             m_sorted( false ) {
    }

    Glossary::Impl::~Impl() {
        for( uint32_t i = 0; i < m_entries.size(); ++i ) {
            EntryBase*& pEntry = m_entries[i];
            delete pEntry;
            pEntry = nullptr;
        }
        m_entries.clear();
    }

    bool Glossary::Impl::RegisterTerm( const char* pTop, const char* pEnd ) {
        if( CheckUniqueness( pTop, pEnd ) == false )
            return false;
        auto pEntry = new GlossaryEntry( pTop, pEnd );
        m_entries.push_back( pEntry );
        m_sorted = false;
        return true;
    }

    bool Glossary::Impl::RegisterAutoLink( const char* pTop, const char* pEnd,
                                           const char* pTargetTop, const char* pTargetEnd ) {
        if( CheckUniqueness( pTop, pEnd ) == false )
            return false;
        EntryBase* pEntry = nullptr;
        const char* const p = pTargetTop;
        if( *p == '#' || (p[1] == '#' && (*p == 'T' || *p == 'F' || *p == 'A')) )
            pEntry = new InternalAutoLinkEntry( pTop, pEnd, pTargetTop, pTargetEnd );
        else
            pEntry = new ExternalAutoLinkEntry( pTop, pEnd, pTargetTop, pTargetEnd );
        m_entries.push_back( pEntry );
        m_sorted = false;
        return true;
    }

    const char* Glossary::Impl::GetAnchorTag( const char* pTerm,
                                              const char* pTermEnd ) const {
        uint64_t hash = CRC64::Calc( 'G', pTerm, pTermEnd ); // 'G' means glossary.
        for( uint32_t i = 0; i < m_entries.size(); ++i ) {
            if( m_entries[i]->GetHash() == hash )
                return m_entries[i]->GetAnchorTag();
        }
        return nullptr;
    }

    void Glossary::Impl::SortIfNeed() {
        if( !m_sorted ) {
            if( m_entries.empty() == false ) {
                std::sort( m_entries.begin(), m_entries.end(),
                           []( const EntryBase* p1, const EntryBase* p2 ) -> bool {
                               return p1->GetLength() > p2->GetLength();
                           } );
            }
            m_sorted = true;
        }
    }

    void Glossary::Impl::WriteWithTermLink( std::ostream& os,
                                            const char* pTop, const char* pEnd,
                                            uint32_t idx, WriteFunction* pWriteFunc ) const {
        for( ; idx < m_entries.size(); ++idx ) {
            EntryBase* pEntry = m_entries[idx];
            auto pTerm1 = pEntry->GetTerm();
            auto length = pEntry->GetLength();
            while( pTop < pEnd ) {
                auto p = std::search( pTop, pEnd, pTerm1, pTerm1 + length );
                if( p == pEnd )
                    break;
                if( pTop < p )
                    this->WriteWithTermLink( os, pTop, p, idx + 1, pWriteFunc );
                pEntry->Write( os, pWriteFunc );
                pTop = p + length;
            }
        }
        pWriteFunc( os, pTop, pEnd );
    }

    void Glossary::Impl::WriteIndex( std::ostream& os, DocumentInfo& docInfo ) const {

        std::vector<IndexEntry> tmpEntries;
        SetupIndexEntries( tmpEntries );

        auto& styles = docInfo.Get<StyleStack>();
        char mark = 0;
        styles.WriteOpenTag( os, "ul" ) << std::endl;
        for( const auto& entry : tmpEntries ) {
            if( entry.first != mark ) {
                if( !!mark )
                    os << "</ul>" << std::endl << "</li>" << std::endl;
                styles.WriteOpenTag( os, "li" ) << GetIndexTag( entry.first ) << std::endl;
                styles.WriteOpenTag( os, "ul" ) << std::endl;
                mark = entry.first;
            }
            styles.WriteOpenTag( os, "li" );
            entry.second->Write( os, TextSpan::WriteWithEscape );
            os << "</li>" << std::endl;
        }
        if( !!mark )
            os << "</ul>" << std::endl
               << "</li>" << std::endl;
        os << "</ul>" << std::endl;
    }

    void Glossary::Impl::WriteIndex_X( std::ostream& os, DocumentInfo& docInfo ) const {

        auto& styles = docInfo.Get<StyleStack>();
        TextSpan listitm{ "li" };
        TextSpan details{ "details" };
        TextSpan summary{ "summary" };
        TextSpan bkquote{ "blockquote" };
        TextSpan clsidxx{ "class='idx_x'" };
        styles.PushStyle( listitm, clsidxx );
        styles.PushStyle( details, clsidxx );
        styles.PushStyle( summary, clsidxx );
        styles.PushStyle( bkquote, clsidxx );

        std::vector<IndexEntry> tmpEntries;
        SetupIndexEntries( tmpEntries );
        char mark = 0;
        for( const auto& entry : tmpEntries ) {
            if( entry.first != mark ) {
                if( !!mark )
                    os << "</blockquote>" << std::endl << "</details>" << std::endl;
                styles.WriteOpenTag( os, "details", nullptr, " open>" ) << std::endl;
                styles.WriteOpenTag( os, "summary" ) << GetIndexTag( entry.first )
                                                             << "</summary>" << std::endl;
                styles.WriteOpenTag( os, "blockquote" ) << std::endl;
                mark = entry.first;
            }
            styles.WriteOpenTag( os, "li" );
            entry.second->Write( os, TextSpan::WriteWithEscape );
            os << "</li>" << std::endl;
        }
        if( !!mark )
            os << "</blockquote>" << std::endl
               << "</details>" << std::endl;

        styles.PopStyle( listitm );
        styles.PopStyle( details );
        styles.PopStyle( summary );
        styles.PopStyle( bkquote );
    }

    bool Glossary::Impl::CheckUniqueness( const char* pTop, const char* pEnd ) const {
        uint32_t len = pEnd - pTop;
        for( uint32_t idx = 0; idx < m_entries.size(); ++idx ) {
            EntryBase* pEntry = m_entries[idx];
            if( len == pEntry->GetLength() &&
                        !::strncmp( pTop, pEntry->GetTerm(), len ) )
                return false;
        }
        return true;
    }

    void Glossary::Impl::SetupIndexEntries( std::vector<IndexEntry>& container ) const {
        container.reserve( m_entries.size() );
        std::transform( m_entries.begin(), m_entries.end(),
                        std::back_inserter( container ), CreateIndexEntry );
        std::sort( container.begin(), container.end(), CompareIndexEntry );
    }

    #define MARK_SYMBOL  '!'  // 0x21
    #define MARK_NUMBER  '0'  // 0x30
    #define MARK_OTHERS  '~'  // 0x7E

    Glossary::Impl::IndexEntry Glossary::Impl::CreateIndexEntry( const EntryBase* pEntry ) {
        auto p = reinterpret_cast<const unsigned char*>( pEntry->GetTerm() );
        char mark;
             if( 'a' <= *p && *p <= 'z' )   mark = 'A' + (*p - 'a');
        else if( 'A' <= *p && *p <= 'Z' )   mark = *p;
        else if( '0' <= *p && *p <= '9' )   mark = MARK_NUMBER;
        else if( *p < 128 )                 mark = MARK_SYMBOL;
        else                                mark = MARK_OTHERS;
        return std::make_pair( mark, pEntry );
    }

    bool Glossary::Impl::CompareIndexEntry( const IndexEntry& e1,
                                            const IndexEntry& e2 ) {
        if( e1.first < e2.first ) return true;
        if( e1.first > e2.first ) return false;
        auto p1 = e1.second->GetTerm();
        auto p2 = e2.second->GetTerm();
        return std::lexicographical_compare( p1, p1 + e1.second->GetLength(),
                                             p2, p2 + e2.second->GetLength() );
    }

    const char* Glossary::Impl::GetIndexTag( char mark ) {
        static char buf[2] = { 0, 0 };
        switch( mark ) {
        case MARK_SYMBOL: return "Symbols";
        case MARK_NUMBER: return "Numbers";
        case MARK_OTHERS: return "Others";
        default:
            buf[0] = mark;
            return buf;
        }
    }

    #undef MARK_SYMBOL
    #undef MARK_NUMBER
    #undef MARK_OTHERS

} // namespace turnup

