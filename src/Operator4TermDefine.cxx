//------------------------------------------------------------------------------
//
// Operator4TermDefine.cxx
//
//------------------------------------------------------------------------------
#include "Operator4TermDefine.hxx"

#include "DocumentInfo.hxx"
#include "Glossary.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "Utilities.hxx"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

namespace turnup {

bool IsTermDefine(const TextSpan& line,
                  const char*& pTop, const char*& pEnd) {
    if (line.BeginWith("*[") == false)
        return false;
    const char* p1 = line.Top();
    const char* p3 = line.End();
    const char* target = "]:";
    const char* p2 = std::search(p1 + 2, p3, target, target + 2);
    if (p2 == p3)
        return false;
    pTop = p1 +2;
    pEnd = p2;
    return true;
}

const TextSpan* Operator4TermDefine(const TextSpan* pTop,
                                    const TextSpan* pEnd, DocumentInfo& docInfo) {
    const char* p1;
    const char* p2;
    if (IsTermDefine(*pTop, p1, p2) == false)
        return pTop;

    auto& styles = docInfo.Get<StyleStack>();
    auto& glossary = docInfo.Get<Glossary>();
    const char* pAnchor = nullptr;
    {
        const char* pTermTop = p1;
        const char* pTermEnd = p2;
        Utilities::Trim(pTermTop, pTermEnd);
        bool bDuplicated = false;
        pAnchor = glossary.GetAnchorTag(bDuplicated, pTermTop, pTermEnd);
        // Duplicated な用語定義でも、ここではリンク先を生成するだけなのでエラーにはしない
        styles.WriteOpenTag(std::cout, "dl") << std::endl;
        styles.WriteOpenTag(std::cout, "dt");
        std::cout << "<a name='" << pAnchor <<  "'></a>";
        std::cout.write(pTermTop, pTermEnd - pTermTop);
        std::cout << "</dt>" << std::endl;
    }
    std::ostringstream os;
    styles.WriteOpenTag(std::cout, "dd"); {
        TextSpan line = *pTop;
        line = line.Chomp((p2+2) - pTop->Top(), 0).TrimHead();
        line.WriteTo(std::cout, docInfo);
        line.WriteTo(os, docInfo);
        // 行末に 2 つスペースがある場合は <br> を追加で出力する
        if (line.EndWith("  "))
            std::cout << "<br>";
        while (++pTop < pEnd) {
            line = pTop->TrimHead();
            if (line.IsEmpty())
                break;
            line.WriteTo(std::cout, docInfo);
            line.WriteTo(os, docInfo);
            // 行末に 2 つスペースがある場合は <br> を追加で出力する
            if (line.EndWith("  "))
                std::cout << "<br>";
        }
    }

    std::string tips = os.str();
    {  // Common with TextSpan.cxx::GenerateFootnoteTips()
        bool in_tag = false;
        auto pred = [&in_tag](char c) -> bool {
            if (in_tag) {
                if (c == '>') {
                    in_tag = false;
                    return true;
                }
            } else {
                if (c == '<')
                    in_tag = true;
            }
            return in_tag;
        };
        auto itr = std::remove_if(tips.begin(), tips.end(), pred);
        tips.erase(itr, tips.end());
    }
    glossary.RegisterTermTips(pAnchor, tips);
    std::cout << "</dd>" << std::endl;
    std::cout << "</dl>" << std::endl;
    return pTop;
}


}  // namespace turnup

