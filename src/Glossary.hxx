//------------------------------------------------------------------------------
//
// Glossary.hxx
//
//------------------------------------------------------------------------------
#ifndef GLOSSARY_HXX__
#define GLOSSARY_HXX__

#include <iosfwd>

namespace turnup {

    class DocumentInfo;

    typedef void WriteFunction( std::ostream&, const char*, const char* );

    //--------------------------------------------------------------------------
    //
    // class Glossary
    //
    //--------------------------------------------------------------------------
    class Glossary {
    public:
        Glossary();
        ~Glossary();
    public:
        bool RegisterTerm( const char* pTop, const char* pEnd );
        bool RegisterAutoLink( const char* pTop, const char* pEnd,
                               const char* pUrlTop, const char* pUrlEnd );
        const char* GetAnchorTag( const char* pTerm,
                                  const char* pTermEnd = nullptr ) const;
        void WriteWithTermLink( std::ostream& os, const char* pTop,
                                const char* pEnd, WriteFunction* pWriteFunc ) const;
        void WriteIndex( std::ostream& os,
                         DocumentInfo& docInfo, bool bFoldable ) const;
    private:
        class Impl;
        Impl* m_pImpl;
    };

} // namespace turnup

#endif // GLOSSARY_HXX__
