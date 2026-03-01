//------------------------------------------------------------------------------
//
// ImageModeStack.hxx
//
//------------------------------------------------------------------------------
#ifndef IMAGEMODESTACK_HXX__
#define IMAGEMODESTACK_HXX__

#include <iosfwd>

namespace turnup {

    enum class ImageMode {
        LINK  = 0,
        EMBED = 1
    };

    //--------------------------------------------------------------------------
    //
    // class ImageModeStack
    //
    //--------------------------------------------------------------------------
    class ImageModeStack {
    public:
        ImageModeStack();
        ~ImageModeStack();
    public:
        void PushMode( ImageMode mode );
        bool PopMode();
        ImageMode GetCurrentMode() const;
    private:
        class Impl;
        Impl* m_pImpl;
    };

} // namespace turnup

#endif // IMAGEMODESTACK_HXX__
