//------------------------------------------------------------------------------
//
// LineType.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_LINETYPE_HXX_
#define SRC_LINETYPE_HXX_

namespace turnup {

enum LineType {
    EMPTY       = 0,
    HEADER      = 1,
    DOT_LIST    = 2,
    NUM_LIST    = 3,
    PRE_BLOCK   = 4,
    TABLE       = 5,
    BLOCKQUOTE  = 6,
    TERM_DEF    = 7,
    COMMENT     = 8,
    HR_LINE     = 15,
    IMAGE       = 16,
    PARAGRAPH   = 0xFFFFFFFF
};

class TextSpan;
LineType GetLineType(const TextSpan* pLine);

}  // namespace turnup

#endif  // SRC_LINETYPE_HXX_
