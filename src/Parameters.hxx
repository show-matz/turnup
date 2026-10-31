//------------------------------------------------------------------------------
//
// Parameters.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_PARAMETERS_HXX_
#define SRC_PARAMETERS_HXX_

#include <stdint.h>

namespace turnup {

class TextSpan;

//--------------------------------------------------------------------------
//
// class Parameters
//
//--------------------------------------------------------------------------
class Parameters {
 public:
    Parameters();
    ~Parameters();

 public:
    bool Analyze(int argc, char* argv[]);

 public:
    bool GetTargetFile(TextSpan& ref) const;
    bool VersionMode() const;    // --version
    bool SafeMode() const;       // --version
    uint32_t DefinitionCount() const;
    bool Definition(uint32_t idx, TextSpan& ref) const;
    const TextSpan* IncludePathTop() const;
    const TextSpan* IncludePathEnd() const;
    const char* GetCrcSalt() const;
    bool NeedWarning() const;

 private:
    class Impl;
    Impl* m_pImpl;
};

}  // namespace turnup

#endif  // SRC_PARAMETERS_HXX_
