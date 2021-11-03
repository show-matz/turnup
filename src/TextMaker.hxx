//------------------------------------------------------------------------------
//
// TextMaker.hxx
//
//------------------------------------------------------------------------------
#ifndef TEXTMAKER_HXX__
#define TEXTMAKER_HXX__

namespace turnup {

	class TextSpan;

	//--------------------------------------------------------------------------
	//
	// class TextMaker
	//
	//--------------------------------------------------------------------------
	class TextMaker {
		friend TextMaker& operator<<( TextMaker& tm, const char* p );
		friend TextMaker& operator<<( TextMaker& tm, const TextSpan& span );
	public:
		class Param;
	public:
		TextMaker();
		~TextMaker();
	public:
		TextSpan GetSpan() const;
	private:
		Param* m_pLast;
	};

	TextMaker& operator<<( TextMaker& tm, const char* p );
	TextMaker& operator<<( TextMaker& tm, const TextSpan& span );


} // namespace turnup

#endif // TEXTMAKER_HXX__
