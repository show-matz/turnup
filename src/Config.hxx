//------------------------------------------------------------------------------
//
// Config.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_HXX_
#define SRC_CONFIG_HXX_

#include <stdint.h>

namespace turnup {

//--------------------------------------------------------------------------
//
// class Config
//
//--------------------------------------------------------------------------
class Config {
 public:
    Config();
    ~Config();

 public:
    bool    bTermLinkInHeader;
    bool    bEmbedStyleSheet;
    bool    bWriteComment;
    bool    bNumberingHeader;
    bool    bUseMathJax;
    bool    bEntityNumbering;
    uint8_t minNumberingLv;
    uint8_t maxNumberingLv;
    uint8_t entityNumberingDepth;
};

}  // namespace turnup

#endif  // SRC_CONFIG_HXX_
