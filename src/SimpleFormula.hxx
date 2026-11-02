//------------------------------------------------------------------------------
//
// SimpleFormula.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_SIMPLEFORMULA_HXX_
#define SRC_SIMPLEFORMULA_HXX_

#include <stdint.h>

namespace turnup {

//--------------------------------------------------------------------------
//
// class SimpleFormula
//
//--------------------------------------------------------------------------
class SimpleFormula {
 public:
    typedef void CallbackFunc(const char* p, uint32_t len, void* dum);
    static void Expand(const char* p1, const char* p2,
                       CallbackFunc* pCallback, void* dum);
};

}  // namespace turnup

#endif  // SRC_SIMPLEFORMULA_HXX_
