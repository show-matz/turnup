//------------------------------------------------------------------------------
//
// StyleStack.hxx
//
//------------------------------------------------------------------------------
#ifndef STYLESTACK_HXX__
#define STYLESTACK_HXX__

#include <iosfwd>

namespace turnup {

    class TextSpan;

    //--------------------------------------------------------------------------
    //
    // class StyleStack
    //
    //--------------------------------------------------------------------------
    class StyleStack {
    public:
        StyleStack();
        ~StyleStack();
    public:
        void PushStyle( const TextSpan& tagName, const TextSpan& style );
        bool PopStyle( const TextSpan& tagName );
        std::ostream& WriteOpenTag( std::ostream& os,
                                    const char* pTagName,
                                    const char* pDefault = nullptr,
                                    const char* pTail = nullptr ) const;
    private:
        class Impl;
        Impl* m_pImpl;
    };

} // namespace turnup

#endif // STYLESTACK_HXX__
