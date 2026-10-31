//------------------------------------------------------------------------------
//
// StringReplacer.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_STRINGREPLACER_HXX_
#define SRC_STRINGREPLACER_HXX_

#include "TextSpan.hxx"

namespace turnup {

//--------------------------------------------------------------------------
//
// class StringReplacer
//
//--------------------------------------------------------------------------
class StringReplacer {
 public:
    // replace %1~%9 to [pTop, pEnd)
    StringReplacer(const TextSpan* pTop, const TextSpan* pEnd);
    ~StringReplacer();

 public:
    TextSpan operator()(const TextSpan& line);

 private:
    const TextSpan* const m_pTop;
    const TextSpan* const m_pEnd;
};

}  // namespace turnup

#endif  // SRC_STRINGREPLACER_HXX_
