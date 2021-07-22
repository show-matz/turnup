//------------------------------------------------------------------------------
//
// Filters.hxx
//
//------------------------------------------------------------------------------
#ifndef FILTERS_HXX__
#define FILTERS_HXX__

#include <iosfwd>

namespace turnup {

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
		void RegistFilter( const TextSpan& label, const TextSpan& command );
		bool ExecuteFilter( std::ostream& os, const TextSpan& type,
							const TextSpan* pTop, const TextSpan* pEnd );
	private:
		class Impl;
		Impl* m_pImpl;
	};

} // namespace turnup

#endif // FILTERS_HXX__
