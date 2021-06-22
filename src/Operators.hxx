//------------------------------------------------------------------------------
//
// Operators.hxx
//
//------------------------------------------------------------------------------
#ifndef OPERATORS_HXX__
#define OPERATORS_HXX__

namespace turnup {

	class TextSpan;
	class DocumentInfo;
	typedef const TextSpan* OperatorType( const TextSpan* pTop,
										  const TextSpan* pEnd, DocumentInfo& docInfo );

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
		const TextSpan* OperateLines( const TextSpan* pTop,
									  const TextSpan* pEnd, DocumentInfo& docInfo );
	};


} // namespace turnup

#endif // OPERATORS_HXX__
