//------------------------------------------------------------------------------
//
// Glossary.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_GLOSSARY_HXX_
#define SRC_GLOSSARY_HXX_

#include <iosfwd>
#include <string>

namespace turnup {

class DocumentInfo;

typedef void WriteFunction(std::ostream&, const char*, const char*);

//--------------------------------------------------------------------------
//
// class Glossary
//
//--------------------------------------------------------------------------
class Glossary {
 public:
    Glossary();
    ~Glossary();

 public:
    bool RegisterTerm(const char* pTop, const char* pEnd);
    void RegisterTermTips(const char* pAnchor, const std::string& tips);
    bool RegisterAutoLink(const char* pTop, const char* pEnd,
                          const char* pUrlTop, const char* pUrlEnd);
    const char* GetAnchorTag(bool& bDuplicated,
                             const char* pTerm, const char* pTermEnd = nullptr) const;
    void WriteWithTermLink(std::ostream& os, const char* pTop,
                           const char* pEnd, WriteFunction* pWriteFunc) const;
    void WriteIndex(std::ostream& os,
                    DocumentInfo& docInfo, bool bFoldable) const;

 private:
    class Impl;
    Impl* m_pImpl;
};

}  // namespace turnup

#endif  // SRC_GLOSSARY_HXX_
