//------------------------------------------------------------------------------
//
// Filters.hxx
//
//------------------------------------------------------------------------------
#ifndef SRC_FILTERS_HXX_
#define SRC_FILTERS_HXX_

#include <iosfwd>

namespace turnup {

class DocumentInfo;
class TextSpan;

//--------------------------------------------------------------------------
//
// class Filters
//
//--------------------------------------------------------------------------
class Filters {
 public:
    Filters();
    ~Filters();

 public:
    void RegistExternal(const TextSpan& label, const TextSpan& command);
    void RegistDefault(const TextSpan& command);
    bool ExecuteFilter(std::ostream& os,
                       DocumentInfo& docInfo,
                       const TextSpan& type,
                       const TextSpan* pTop, const TextSpan* pEnd);

 private:
    class Impl;
    Impl* m_pImpl;
};

}  // namespace turnup

#endif  // SRC_FILTERS_HXX_
