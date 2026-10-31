//------------------------------------------------------------------------------
//
// StyleStack.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_STYLESTACK_HXX_
#define SRC_STYLESTACK_HXX_

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
    void PushStyle(const TextSpan& tagName, const TextSpan& style);
    bool PopStyle(const TextSpan& tagName);
    std::ostream& WriteOpenTag(std::ostream& os,
                               const char* pTagName,
                               const char* pDefault = nullptr,
                               const char* pTail = nullptr) const;

 private:
    class Impl;
    Impl* m_pImpl;
};

}  // namespace turnup

#endif  // SRC_STYLESTACK_HXX_
