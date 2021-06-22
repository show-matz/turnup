//------------------------------------------------------------------------------
//
// InputData.cxx
//
//------------------------------------------------------------------------------
#include "InputData.hxx"

#include "DocumentInfo.hxx"
#include "HtmlHeader.hxx"
#include "Config.hxx"
#include "ToC.hxx"
#include "Glossary.hxx"
#include "File.hxx"
#include "Operator4TermDefine.hxx"

#include <vector>
#include <algorithm>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class InputDataImpl
	//
	//--------------------------------------------------------------------------
	class InputDataImpl : public InputData {
	public:
		InputDataImpl( const char* pFileName );
		virtual ~InputDataImpl();
	public:
		virtual uint32_t Size() const override;
		virtual const TextSpan* Begin() const override;
		virtual const TextSpan* End() const override;
		virtual void PreScan( DocumentInfo& docInfo ) override;
	private:
		WholeFile*				m_pFileData;
		std::vector<TextSpan>	m_lines;
	};

	//--------------------------------------------------------------------------
	//
	// implementation of class InputData
	//
	//--------------------------------------------------------------------------
	InputData::InputData() {
	}

	InputData::~InputData() {
	}

	InputData* InputData::Create( const char* pFileName ) {
		return new InputDataImpl{ pFileName };
	}

	void InputData::Release( InputData* pInputData ) {
		delete pInputData;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class InputDataImpl
	//
	//--------------------------------------------------------------------------
	InputDataImpl::InputDataImpl( const char* pFileName ) : InputData(),
															m_pFileData( File::LoadWhole( pFileName ) ),
															m_lines( ) {
		m_lines.reserve( 1000 );	//ToDo : ok?
		char* pTop = m_pFileData->GetBuffer<char>();
		char* pEnd = pTop + m_pFileData->Count<char>();
		while( pTop < pEnd ) {
			char* pEOL = std::find( pTop, pEnd, 0x0A );
			char* pNext = pEOL + 1;
			pEOL[0] = 0;
			if( pEOL[-1] == 0x0D )
				*--pEOL = 0;
			m_lines.emplace_back( pTop, pEOL );
			pTop = pNext;
		}
	}

	InputDataImpl::~InputDataImpl() {
		m_lines.clear();
		File::ReleaseWholeFile( m_pFileData );
	}

	uint32_t InputDataImpl::Size() const {
		return m_lines.size();
	}

	const TextSpan* InputDataImpl::Begin() const {
		if( m_lines.empty() )
			return nullptr;
		return &(m_lines[0]);
	}

	const TextSpan* InputDataImpl::End() const {
		if( m_lines.empty() )
			return nullptr;
		return &(m_lines[0]) + this->Size();
	}

	void InputDataImpl::PreScan( DocumentInfo& docInfo ) {

		auto& header   = docInfo.Get<HtmlHeader>();
		auto& toc      = docInfo.Get<ToC>();
		auto& glossary = docInfo.Get<Glossary>();

		auto itr1 = m_lines.begin();
		auto itr2 = m_lines.end();

		for( ; itr1 != itr2; ++itr1 ) {
			TextSpan& line = *itr1;
			//見出しであれば ToC に登録する
			uint32_t lv = line.CountTopOf('#');
			if( 0 < lv && lv <= 6 && line[lv] == ' ' ) {
				const char* pTitle = line.Top() + lv + 1;
				toc.Register( lv, pTitle );
				continue;
			}
			//用語定義の処理
			if( line.BeginWith( "*[" ) ) {
				const char* pTermTop;
				const char* pTermEnd;
				if( IsTermDefine( line, pTermTop, pTermEnd ) )
					glossary.Register( pTermTop, pTermEnd );
				continue;
			}
			//コメントの場合
			TextSpan tmp = line.TrimTail();
			TextSpan item;
			if( tmp.IsMatch( "<!-- title:", item, " -->" ) ) {
				const_cast<char*>( item.End() )[0] = 0;
				header.SetTitle( item.Top() );
				line.Clear();
			}
			if( tmp.IsMatch( "<!-- style:", item, " -->" ) ) {
				const_cast<char*>( item.End() )[0] = 0;
				header.SetStyleSheet( item.Top() );
				line.Clear();
			}
			if( tmp.IsMatch( "<!-- config:", item, " -->" ) ) {
				auto& cfg = docInfo.Get<Config>();
				if( item.IsEqual( "term-link-in-header" ) ) {
					cfg.bTermLinkInHeader = true;
				} else if( item.BeginWith( "header-numbering" ) ) {
					item = item.Chomp( 16, 0 ).Trim();
					uint32_t lvls[] = { 1, 6 };
					for( uint32_t i = 0; i < 2; ++i ) {
						if( item.IsEmpty() == false ) {
							TextSpan token = item.CutNextToken( ' ' );
							if( token.Convert( lvls[i] ) == false ) {
								//ToDo : error message...
							}
						}
						item = item.Trim();
					}
					if( lvls[0] < 1 || 6 < lvls[1] || lvls[1] < lvls[0] ) {
						//ToDo : error message...
					}
					cfg.bNumberingHeader = true;
					cfg.minNumberingLv   = lvls[0];
					cfg.maxNumberingLv   = lvls[1];
				} else {
					//ToDo : error message...?
				}
			}
		}
	}

} // namespace turnup
