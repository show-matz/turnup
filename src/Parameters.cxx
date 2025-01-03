//------------------------------------------------------------------------------
//
// Parameters.cxx
//
//------------------------------------------------------------------------------
#include "Parameters.hxx"

#include "TextSpan.hxx"

#include <string.h>
#include <vector>

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class of class Parameters::Impl
    //
    //--------------------------------------------------------------------------
    class Parameters::Impl {
    public:
        Impl();
        ~Impl();
    public:
        bool Analyze( int argc, char* argv[] );
    public:
        bool GetTargetFile( TextSpan& ref ) const;
        bool VersionMode() const;    // --version
        uint32_t DefinitionCount() const;
        bool Definition( uint32_t idx, TextSpan& ref ) const;
        const TextSpan* IncludePathTop() const;
        const TextSpan* IncludePathEnd() const;
        const char* GetCrcSalt() const;
    private:
        typedef std::vector<TextSpan> Definitions;
        typedef std::vector<TextSpan> IncludePaths;
    public:
        TextSpan     m_inputFile;
        bool         m_versionMode;
        Definitions  m_definitions;
        IncludePaths m_includePaths;
        const char*  m_pCrcSalt;
    };

    //--------------------------------------------------------------------------
    //
    // implementation of class Parameters
    //
    //--------------------------------------------------------------------------
    Parameters::Parameters() : m_pImpl( new Impl{} ) {
    }
    Parameters::~Parameters() {
        delete m_pImpl;
    }
    bool Parameters::Analyze( int argc, char* argv[] ) {
        return m_pImpl->Analyze( argc, argv );
    }
    bool Parameters::GetTargetFile( TextSpan& ref ) const {
        return m_pImpl->GetTargetFile( ref );
    }
    bool Parameters::VersionMode() const {
        return m_pImpl->VersionMode();
    }
    uint32_t Parameters::DefinitionCount() const {
        return m_pImpl->DefinitionCount();
    }
    bool Parameters::Definition( uint32_t idx, TextSpan& ref ) const {
        return m_pImpl->Definition( idx, ref );
    }
    const TextSpan* Parameters::IncludePathTop() const {
        return m_pImpl->IncludePathTop();
    }
    const TextSpan* Parameters::IncludePathEnd() const {
        return m_pImpl->IncludePathEnd();
    }
    const char* Parameters::GetCrcSalt() const {
        return m_pImpl->GetCrcSalt();
    }

    //--------------------------------------------------------------------------
    //
    // implementation of class Parameters::Impl
    //
    //--------------------------------------------------------------------------
    Parameters::Impl::Impl() : m_inputFile(),
                               m_versionMode( false ),
                               m_definitions(),
                               m_includePaths(),
                               m_pCrcSalt( 0 ) {
    }

    Parameters::Impl::~Impl() {
        m_definitions.clear();
        m_includePaths.clear();
    }

    bool Parameters::Impl::Analyze( int argc, char* argv[] ) {
        int i = 1;
        for( ; i < argc; ++i ) {
            const char* p = argv[i];
            if( !::strcmp( p, "--version" ) ) {
                m_versionMode = true;
            } else if( !::strncmp( p, "-D", 2 ) ) {
                m_definitions.emplace_back( p, p + ::strlen( p ) );
            } else if( !::strncmp( p, "-I", 2 ) ) {
                m_includePaths.emplace_back( p + 2, p + ::strlen( p ) );
            } else if( !::strncmp( p, "--salt=", 7 ) ) {
                m_pCrcSalt = p + 7;
            } else {
                break;
            }
        }
        if( i == argc )
            return m_versionMode;
        {
            const char* p = argv[i++];
            m_inputFile = TextSpan{ p, p + ::strlen( p ) };
        }
        return ( i == argc );
    }

    bool Parameters::Impl::GetTargetFile( TextSpan& ref ) const {
        if( m_inputFile.IsEmpty() )
            return false;
        ref = m_inputFile;
        return true;
    }

    bool Parameters::Impl::VersionMode() const {
        return m_versionMode;
    }

    uint32_t Parameters::Impl::DefinitionCount() const {
        return m_definitions.size();
    }

    bool Parameters::Impl::Definition( uint32_t idx, TextSpan& ref ) const {
        if( m_definitions.size() <= idx )
            return false;
        ref = m_definitions[idx];
        return true;
    }

    const TextSpan* Parameters::Impl::IncludePathTop() const {
        if( m_includePaths.empty() )
            return nullptr;
        else
            return &(m_includePaths[0]);
    }
    const TextSpan* Parameters::Impl::IncludePathEnd() const {
        if( m_includePaths.empty() )
            return nullptr;
        else
            return &(m_includePaths[0]) + m_includePaths.size();
    }
    const char* Parameters::Impl::GetCrcSalt() const {
        return m_pCrcSalt;
    }

} // namespace turnup

