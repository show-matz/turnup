//------------------------------------------------------------------------------
//
// DocumentInfo.hxx
//
//------------------------------------------------------------------------------
#ifndef DOCUMENTINFO_HXX__
#define DOCUMENTINFO_HXX__

namespace turnup {

    class HtmlHeader;
    class Config;
    class ToC;
    class Glossary;
    class Footnotes;
    class Filters;
    class StyleStack;
    class StylePalette;
    class ImageModeStack;

    //--------------------------------------------------------------------------
    //
    // class DocumentInfo
    //
    //--------------------------------------------------------------------------
    class DocumentInfo {
    public:
        DocumentInfo( bool bSafeMode );
        ~DocumentInfo();
    public:
        void SetCurrentHeader( const char* pTag );
        const char* GetCurrentHeader() const;
    public:
        inline bool IsSafeMode() const { return m_bSafeMode; }
    public:
        template <typename T> T& Get();
        template <typename T> const T& Get() const;
    private:
        class Impl;
        Impl* m_pImpl;
        bool  m_bSafeMode;
    };

} // namespace turnup

#endif // DOCUMENTINFO_HXX__
