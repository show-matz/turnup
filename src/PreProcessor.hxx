//------------------------------------------------------------------------------
//
// PreProcessor.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_PREPROCESSOR_HXX_
#define SRC_PREPROCESSOR_HXX_

namespace turnup {

class TextSpan;

//--------------------------------------------------------------------------
//
// class PreProcessor
//
//--------------------------------------------------------------------------
class PreProcessor {
 protected:
    PreProcessor();
    virtual ~PreProcessor();

 public:
    PreProcessor(const PreProcessor&) = delete;
    PreProcessor& operator=(const PreProcessor&) = delete;

 public:
    virtual bool Execute(TextSpan* pLineTop, TextSpan* pLineEnd) = 0;
    virtual bool RegisterVariable(const TextSpan& name, const TextSpan& value) = 0;

 public:
    static PreProcessor* Create();
    static void Release(PreProcessor* pPreProcessor);
};

}  // namespace turnup

#endif  // SRC_PREPROCESSOR_HXX_
