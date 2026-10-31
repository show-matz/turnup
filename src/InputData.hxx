//------------------------------------------------------------------------------
//
// InputData.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_INPUTDATA_HXX_
#define SRC_INPUTDATA_HXX_

#include <stdint.h>

namespace turnup {

class DocumentInfo;
class PreProcessor;
class TextSpan;

//--------------------------------------------------------------------------
//
// class InputData
//
//--------------------------------------------------------------------------
class InputData {
 protected:
    InputData();
    virtual ~InputData();

 public:
    virtual uint32_t Size() const = 0;
    virtual const TextSpan* Begin() const = 0;
    virtual const TextSpan* End() const = 0;
    virtual bool PreProcess(PreProcessor* pPreProsessor) = 0;
    virtual void PreScan(DocumentInfo& docInfo) = 0;

 public:
    static InputData* Create(const TextSpan& fileName,
                             const TextSpan* pIncPathTop, const TextSpan* pIncPathEnd);
    static void Release(InputData* pInputData);
};

}  // namespace turnup

#endif  // SRC_INPUTDATA_HXX_
