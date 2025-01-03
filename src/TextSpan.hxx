//------------------------------------------------------------------------------
//
// TextSpan.hxx
//
//------------------------------------------------------------------------------
#ifndef TEXTSPAN_HXX__
#define TEXTSPAN_HXX__

#include <stdint.h>
#include <iosfwd>

namespace turnup {

    class DocumentInfo;

    //--------------------------------------------------------------------------
    //
    // class TextSpan
    //
    //--------------------------------------------------------------------------
    class TextSpan {
    public:
        TextSpan();
        TextSpan( const TextSpan& ) = default;
        explicit TextSpan( const char* p );
        TextSpan( const char* pTop, const char* pEnd );
        ~TextSpan();
    public:
        inline uint32_t ByteLength() const { return m_pEnd - m_pTop; }
        inline const char*& Top() { return m_pTop; }
        inline const char*& End() { return m_pEnd; }
        inline const char* Top() const { return m_pTop; }
        inline const char* End() const { return m_pEnd; }
        inline char operator[]( uint32_t idx ) const { return m_pTop[idx]; }
        inline bool IsAsciz() const { return m_pEnd && !*m_pEnd; }
        inline bool IsQuoted() const {
            return 2 <= (m_pEnd - m_pTop)
                     && (m_pTop[0]  == 0x27 || m_pTop[0]  == 0x22)
                     && (m_pEnd[-1] == 0x27 || m_pEnd[-1] == 0x22);
        }
    public:
        bool IsEmpty() const;
        TextSpan Trim() const;
        TextSpan TrimHead() const;
        TextSpan TrimTail() const;
        bool IsMatch( const char* pHead, TextSpan& item, const char* pTail ) const;
        bool IsMatch( const char* pHead, TextSpan& item1,
                      const char* pDelim, TextSpan& item2, const char* pTail ) const;
        bool IsEqual( const TextSpan& other ) const;
        bool IsEqual( const char* p ) const;
        bool BeginWith( const char* p ) const;
        bool EndWith( const char* p ) const;
        uint32_t CountTopOf( char ch ) const;
        bool Convert( uint32_t& val ) const;
    public:
        TextSpan CutNextToken();
        TextSpan CutNextToken( char delimiter );
        TextSpan& Chomp( uint32_t head, uint32_t tail );
        void Clear();
    public:
        std::ostream& WriteTo( std::ostream& os,
                               DocumentInfo& docInfo, bool bTermLink = true ) const;
        std::ostream& WriteSimple( std::ostream& os ) const;
    public:
        static void WriteWithEscape( std::ostream& os,
                                     const char* pTop, const char* pEnd );
        static void DestructureToken( TextSpan data,
                                      bool(*callback)(TextSpan, void*), void* pOpaque );
    private:
        const char* m_pTop;
        const char* m_pEnd;
    };

    std::ostream& operator<<( std::ostream& os, const TextSpan& txt );

} // namespace turnup

#endif // TEXTSPAN_HXX__
