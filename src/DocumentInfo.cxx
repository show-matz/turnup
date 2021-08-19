//------------------------------------------------------------------------------
//
// DocumentInfo.cxx
//
//------------------------------------------------------------------------------
#include "DocumentInfo.hxx"

#include "HtmlHeader.hxx"
#include "Config.hxx"
#include "ToC.hxx"
#include "Glossary.hxx"
#include "Footnotes.hxx"
#include "Filters.hxx"
#include "StyleStack.hxx"

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class DocumentInfo::Impl
	//
	//--------------------------------------------------------------------------
	class DocumentInfo::Impl {
	public:
		HtmlHeader	m_htmHeader;
		Config		m_config;
		ToC			m_toc;
		Glossary	m_glossary;
		Footnotes	m_footnotes;
		Filters		m_filters;
		StyleStack	m_styles;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class DocumentInfo
	//
	//--------------------------------------------------------------------------
	DocumentInfo::DocumentInfo() : m_pImpl( new Impl{} ) {
	}

	DocumentInfo::~DocumentInfo() {
		delete m_pImpl;
	}

	template <> HtmlHeader& DocumentInfo::Get<HtmlHeader>() { return m_pImpl->m_htmHeader; }
	template <> Config&     DocumentInfo::Get<Config>()     { return m_pImpl->m_config;    }
	template <> ToC&        DocumentInfo::Get<ToC>()        { return m_pImpl->m_toc;       }
	template <> Glossary&   DocumentInfo::Get<Glossary>()   { return m_pImpl->m_glossary;  }
	template <> Footnotes&  DocumentInfo::Get<Footnotes>()  { return m_pImpl->m_footnotes; }
	template <> Filters&    DocumentInfo::Get<Filters>()    { return m_pImpl->m_filters;   }
	template <> StyleStack& DocumentInfo::Get<StyleStack>() { return m_pImpl->m_styles;    }

	template <> const HtmlHeader& DocumentInfo::Get<HtmlHeader>() const { return m_pImpl->m_htmHeader; }
	template <> const Config&     DocumentInfo::Get<Config>() const     { return m_pImpl->m_config;    }
	template <> const ToC&        DocumentInfo::Get<ToC>() const        { return m_pImpl->m_toc;       }
	template <> const Glossary&   DocumentInfo::Get<Glossary>() const   { return m_pImpl->m_glossary;  }
	template <> const Footnotes&  DocumentInfo::Get<Footnotes>() const  { return m_pImpl->m_footnotes; }
	template <> const Filters&    DocumentInfo::Get<Filters>() const    { return m_pImpl->m_filters;   }
	template <> const StyleStack& DocumentInfo::Get<StyleStack>() const { return m_pImpl->m_styles;    }

} // namespace turnup
