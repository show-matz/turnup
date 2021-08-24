//------------------------------------------------------------------------------
//
// StylePalette.cxx
//
//------------------------------------------------------------------------------
#include "StylePalette.hxx"

#include "TextSpan.hxx"

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class StylePalette::Impl
	//
	//--------------------------------------------------------------------------
	class StylePalette::Impl {
	public:
		Impl();
		~Impl();
	public:
		void RegisterStyle( uint32_t index, const TextSpan& style );
		const TextSpan* GetStyle( uint32_t index ) const;
	private:
		TextSpan m_styles[10];
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class StylePalette
	//
	//--------------------------------------------------------------------------
	StylePalette::StylePalette() : m_pImpl( nullptr ) {
	}

	StylePalette::~StylePalette() {
		delete m_pImpl;
	}

	void StylePalette::RegisterStyle( uint32_t index, const TextSpan& style ) {
		if( !m_pImpl )
			m_pImpl = new Impl{};
		m_pImpl->RegisterStyle( index, style );
	}

	const TextSpan* StylePalette::GetStyle( uint32_t index ) const {
		if( !m_pImpl )
			return nullptr;
		return m_pImpl->GetStyle( index );
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class StylePalette::Impl
	//
	//--------------------------------------------------------------------------
	StylePalette::Impl::Impl() {
	}

	StylePalette::Impl::~Impl() {
	}

	void StylePalette::Impl::RegisterStyle( uint32_t index, const TextSpan& style ) {
		if( index < 10 )
			m_styles[index] = style;
	}

	const TextSpan* StylePalette::Impl::GetStyle( uint32_t index ) const {
		if( index < 10 )
			return m_styles + index;
		return nullptr;
	}


} // namespace turnup

