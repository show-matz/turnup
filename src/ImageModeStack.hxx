//------------------------------------------------------------------------------
//
// ImageModeStack.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_IMAGEMODESTACK_HXX_
#define SRC_IMAGEMODESTACK_HXX_

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
    void PushMode(ImageMode mode);
    bool PopMode();
    ImageMode GetCurrentMode() const;

 private:
    class Impl;
    Impl* m_pImpl;
};

}  // namespace turnup

#endif  // SRC_IMAGEMODESTACK_HXX_
