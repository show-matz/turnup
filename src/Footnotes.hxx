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
    class TextSpan;

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
        uint32_t Register( const TextSpan& tag, const TextSpan& note );
    public:
        bool RegisterPrefix( const TextSpan& tag, const TextSpan& prefix );
        void GetPrefix( const TextSpan& tag, TextSpan& prefix ) const;
    public:
        void WriteFootnotes( std::ostream& os, DocumentInfo& docInfo ) const;
        void WriteFootnotes( const TextSpan& tag, 
                             std::ostream& os, DocumentInfo& docInfo ) const;
    private:
        class Impl;
        Impl* m_pImpl;
    };

} // namespace turnup

#endif // FOOTNOTES_HXX__
