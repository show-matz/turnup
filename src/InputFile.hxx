//------------------------------------------------------------------------------
//
// InputFile.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INPUTFILE_HXX_
#define SRC_INPUTFILE_HXX_

#include <stdint.h>

namespace turnup {

class TextSpan;

//--------------------------------------------------------------------------
//
// class InputFile
//
//--------------------------------------------------------------------------
class InputFile {
 public:
    InputFile();
    virtual ~InputFile();

 public:
    virtual const TextSpan& GetFileName() const = 0;
    virtual uint32_t LineSize() const = 0;
    virtual const TextSpan* LineTop() const = 0;
    virtual const TextSpan* LineEnd() const = 0;

 public:
    static InputFile* LoadInputFile(const TextSpan& filePath);
    static void ReleaseInputFile(InputFile* pInputFile);
};


}  // namespace turnup

#endif  // SRC_INPUTFILE_HXX_
