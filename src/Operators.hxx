//------------------------------------------------------------------------------
//
// Operators.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_OPERATORS_HXX_
#define SRC_OPERATORS_HXX_

namespace turnup {

class TextSpan;
class DocumentInfo;
typedef const TextSpan* OperatorType(const TextSpan* pTop,
                                     const TextSpan* pEnd, DocumentInfo& docInfo);

//--------------------------------------------------------------------------
//
// class Operators
//
//--------------------------------------------------------------------------
class Operators {
 public:
    Operators();
    ~Operators();

 public:
    const TextSpan* OperateLines(const TextSpan* pTop,
                                 const TextSpan* pEnd, DocumentInfo& docInfo);
};

}  // namespace turnup

#endif  // SRC_OPERATORS_HXX_
