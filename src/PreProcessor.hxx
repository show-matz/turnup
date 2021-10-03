//------------------------------------------------------------------------------
//
// PreProcessor.hxx
//
//------------------------------------------------------------------------------
#ifndef PREPROCESSOR_HXX__
#define PREPROCESSOR_HXX__

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
		PreProcessor( const PreProcessor& ) = delete;
		PreProcessor& operator=( const PreProcessor& ) = delete;
	public:
		virtual bool Execute( TextSpan* pLineTop, TextSpan* pLineEnd,
							  void (*pCallback)( char*, void* ), void* pOpaque ) = 0;
		virtual bool RegisterVariable( const TextSpan& name, const TextSpan& value ) = 0;
	public:
		static PreProcessor* Create();
		static void Release( PreProcessor* pPreProcessor );
	};


} // namespace turnup

#endif // PREPROCESSOR_HXX__
