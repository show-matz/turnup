//------------------------------------------------------------------------------
//
// FilterBuffer.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_FILTERBUFFER_HXX_
#define SRC_FILTERBUFFER_HXX_

#include "TextSpan.hxx"

#include <stdint.h>

namespace turnup {

class TextSpan;

//--------------------------------------------------------------------------
//
// class FilterBuffer
//
//--------------------------------------------------------------------------
class FilterBuffer {
 public:
    FilterBuffer(const TextSpan* pTop, const TextSpan* pEnd);
    ~FilterBuffer();

 public:
    TextSpan GetBuffer() const;

 private:
    uint32_t m_length;
    char*    m_pBuffer;
};

}  // namespace turnup

#endif  // SRC_FILTERBUFFER_HXX_
