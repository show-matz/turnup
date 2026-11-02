//------------------------------------------------------------------------------
//
// TextMaker.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_TEXTMAKER_HXX_
#define SRC_TEXTMAKER_HXX_

namespace turnup {

class TextSpan;

//--------------------------------------------------------------------------
//
// class TextMaker
//
//--------------------------------------------------------------------------
class TextMaker {
    friend TextMaker& operator<<(TextMaker& tm, const char* p);
    friend TextMaker& operator<<(TextMaker& tm, const TextSpan& span);

 public:
    class Param;

 public:
    TextMaker();
    ~TextMaker();

 public:
    TextSpan GetSpan() const;

 private:
    Param* m_pLast;
};

TextMaker& operator<<(TextMaker& tm, const char* p);
TextMaker& operator<<(TextMaker& tm, const TextSpan& span);

}  // namespace turnup

#endif  // SRC_TEXTMAKER_HXX_
