//------------------------------------------------------------------------------
//
// Operator4Lane.cxx
//
//------------------------------------------------------------------------------
#include "Operator4Lane.hxx"

#include "DocumentInfo.hxx"
#include "TextSpan.hxx"

#include <stdint.h>
#include <string.h>
#include <iostream>

namespace turnup {

//--------------------------------------------------------------------------
//
// external functions
//
//--------------------------------------------------------------------------
const TextSpan* Operator4Lane(const TextSpan* pTop,
                              const TextSpan* pEnd, DocumentInfo& docInfo) {
    (void)pEnd;
    (void)docInfo;

    // <!-- lane: で始まっていない行は対象外なので無視
    if (pTop->BeginWith("<!-- lane:") == false)
        return pTop;

    // --> で終わっていない行も対象外なので無視
    TextSpan line = pTop->TrimTail();
    if (line.EndWith("-->") == false)
        return pTop;

    // 先頭の <!-- lane: と 末尾の --> を除去して更に Trim
    line = line.Chomp(10, 3).Trim();

    // lane: に後続する type を取得（begin/next/end を期待 : そうでないなら無視）
    TextSpan type = line.CutNextToken(' ');
    line = line.Trim();
    if (type.IsEqual("begin") == false &&
             type.IsEqual("next") == false && type.IsEqual("end") == false)
        return pTop;

    if (type.IsEqual("begin")) {
        std::cout << "<table style='border: 0px;'>"
                  << "<tr style='border: 0px;'>"
                  << "<td ";
        if (line.IsEmpty() == false)
            std::cout << "width='" << line << "' ";
        std::cout << "style='border: 0px; vertical-align: top;'>" << std::endl;

    } else if (type.IsEqual("next")) {
        std::cout << "</td>"
                  << "<td ";
        if (line.IsEmpty() == false)
            std::cout << "width='" << line << "' ";
        std::cout << "style='border: 0px; vertical-align: top;'>" << std::endl;

    } else {
        std::cout << "</td></tr></table>" << std::endl;
    }
    return pTop + 1;
}

}  // namespace turnup

