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
#include "Filters.hxx"
#include "File.hxx"
#include "Operator4TermDefine.hxx"
#include "Utilities.hxx"

#include <vector>
#include <algorithm>
#include <iostream>

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
		static char* FixLineEnd( char* pTop, char* pEnd );
		static char* FixLineContinuous( char* pTop, char* pEnd );
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
		pEnd = FixLineEnd( pTop, pEnd );
		pEnd = FixLineContinuous( pTop, pEnd );
		while( pTop < pEnd ) {
			char* pEOL = std::find( pTop, pEnd, 0x0A );
			char* pNext = pEOL + 1;
			pEOL[0] = 0;
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
		auto& filters  = docInfo.Get<Filters>();

		auto itr1 = m_lines.begin();
		auto itr2 = m_lines.end();

		for( ; itr1 != itr2; ++itr1 ) {
			TextSpan& line = *itr1;
			//見出しであれば ToC に登録する
			uint32_t lv = line.CountTopOf('#');
			if( 0 < lv && lv <= 6 && line[lv] == ' ' ) {
				line = line.TrimTail();	// ここで末尾の空白類文字を除去してしまう
				const_cast<char*>( line.End() )[0] = 0;	//HACK
				const char* pTitle = line.Top() + lv + 1;
				if( toc.RegisterHeader( lv, pTitle ) == false )
					std::cerr << "ERROR : header '" << pTitle << "' is duplicated." << std::endl;
				continue;
			}
			TextSpan tmp;
			//図表タイトルであれば ToC に登録する
			if( line.BeginWith( "Table." ) ) {
				line = line.TrimTail();	// ここで末尾の空白類文字を除去してしまう
				const_cast<char*>( line.End() )[0] = 0;	//HACK
				tmp = line;
				tmp = tmp.Chomp( 6, 0 ).Trim();
				if( toc.RegisterTable( tmp.Top() ) == false ) {
					std::cerr << "ERROR : table '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			if( line.BeginWith( "Figure." ) ) {
				line = line.TrimTail();	// ここで末尾の空白類文字を除去してしまう
				const_cast<char*>( line.End() )[0] = 0;	//HACK
				tmp = line;
				tmp = tmp.Chomp( 7, 0 ).Trim();
				if( toc.RegisterFigure( tmp.Top() ) == false ) {
					std::cerr << "ERROR : figure '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			//用語定義の処理
			if( line.BeginWith( "*[" ) ) {
				const char* pTermTop;
				const char* pTermEnd;
				if( IsTermDefine( line, pTermTop, pTermEnd ) ) {
					Utilities::Trim( pTermTop, pTermEnd );
					glossary.Register( pTermTop, pTermEnd );
				}
				continue;
			}
			//コメントの場合
			tmp = line.TrimTail();
			TextSpan item;
			TextSpan command;
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
			if( tmp.IsMatch( "<!-- filter:", item, "=", command, " -->" ) ) {
				filters.RegistExternal( item.Trim(), command.Trim() );
				line.Clear();
			}
			if( tmp.IsMatch( "<!-- config:", item, " -->" ) ) {
				auto& cfg = docInfo.Get<Config>();
				if( item.IsEqual( "term-link-in-header" ) ) {
					cfg.bTermLinkInHeader = true;
				} else if( item.IsEqual( "embed-stylesheet" ) ) {
					cfg.bEmbedStyleSheet = true;
				} else if( item.IsEqual( "write-comment" ) ) {
					cfg.bWriteComment = true;
				} else if( item.BeginWith( "entity-numbering-depth" ) ) {
					item = item.Chomp( 22, 0 ).Trim();
					uint32_t depth = 0;
					if( item.Convert( depth ) == false ) {
						//ToDo : error message...
					}
					cfg.entityNumberingDepth = depth;
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

	char* InputDataImpl::FixLineEnd( char* pTop, char* pEnd ) {
		char target[2] = { 0x0D, 0x0A };
		//ひとつめの CrLf を検索
		char* p = std::search( pTop, pEnd, target, target + 2 );
		if( p == pEnd ) {
			//みつからなければ何もしないでよし
			return pEnd;
		}
		pTop = p + 1;
		char* pDest = p;
		while( pTop < pEnd ) {
			p = std::search( pTop, pEnd, target, target + 2 );
			pDest = std::copy( pTop, p, pDest );
			pTop = p + 1;
		}
		*pDest = 0;
		return pDest;
	}

	char* InputDataImpl::FixLineContinuous( char* pTop, char* pEnd ) {
		char target[3] = { ' ', '\\', 0x0A };
		//ひとつめの継続行を検索
		char* p = std::search( pTop, pEnd, target, target + 3 );
		if( p == pEnd ) {
			//みつからなければ何もしないでよし
			return pEnd;
		}
		pTop = p + 3;
		char* pDest = p;
		while( pTop < pEnd ) {
			p = std::search( pTop, pEnd, target, target + 3 );
			pDest = std::copy( pTop, p, pDest );
			pTop = p + 3;
		}
		*pDest = 0;
		return pDest;
	}

} // namespace turnup
