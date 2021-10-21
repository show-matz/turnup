//------------------------------------------------------------------------------
//
// InputData.cxx
//
//------------------------------------------------------------------------------
#include "InputData.hxx"

#include "Buffer.hxx"
#include "InputFile.hxx"
#include "DocumentInfo.hxx"
#include "HtmlHeader.hxx"
#include "Config.hxx"
#include "ToC.hxx"
#include "Glossary.hxx"
#include "Filters.hxx"
#include "File.hxx"
#include "Operator4TermDefine.hxx"
#include "Utilities.hxx"
#include "PreProcessor.hxx"

#include <vector>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <assert.h>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class InputDataImpl
	//
	//--------------------------------------------------------------------------
	class InputDataImpl : public InputData {
	private:
		typedef std::vector<InputFile*> InputFileStack;
	public:
		InputDataImpl( const TextSpan& fileName );
		virtual ~InputDataImpl();
	public:
		virtual uint32_t Size() const override;
		virtual const TextSpan* Begin() const override;
		virtual const TextSpan* End() const override;
		virtual bool PreProcess( PreProcessor* pPreProsessor ) override;
		virtual void PreScan( DocumentInfo& docInfo ) override;
	private:
		InputFile* LoadFileIfNeed( const TextSpan& fileName );
		void RecursiveLoadFile( InputFileStack& stack, const TextSpan& fileName );
		void AddErrorLine( const char* msg, const TextSpan& fileName );
	private:
		std::vector<InputFile*>	m_inFiles;
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

	InputData* InputData::Create( const TextSpan& fileName ) {
		return new InputDataImpl{ fileName };
	}

	void InputData::Release( InputData* pInputData ) {
		delete pInputData;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class InputDataImpl
	//
	//--------------------------------------------------------------------------
	InputDataImpl::InputDataImpl( const TextSpan& fileName ) : InputData(),
															   m_inFiles(),
															   m_lines() {
		m_lines.reserve( 1000 );	//ToDo : ok?
		InputFileStack stack;
		RecursiveLoadFile( stack, fileName );
	}

	InputDataImpl::~InputDataImpl() {
		m_lines.clear();
		auto itr1 = m_inFiles.begin();
		auto itr2 = m_inFiles.begin();
		for( ; itr1 != itr2; ++itr1 ) {
			InputFile::ReleaseInputFile( *itr1 );
			*itr1 = nullptr;
		}
		m_inFiles.clear();
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

	bool InputDataImpl::PreProcess( PreProcessor* pPreProsessor ) {
		if( m_lines.empty() )
			return true;
		TextSpan* pTop = &(m_lines[0]);
		TextSpan* pEnd = pTop + m_lines.size();
		return pPreProsessor->Execute( pTop, pEnd );
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
				TextSpan tmp = line.TrimTail();	// 末尾の空白類文字を除去
				tmp.Chomp( lv + 1, 0 );
				if( toc.RegisterHeader( lv, tmp ) == false ) {
					std::cerr << "ERROR : header '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			//図表タイトルであれば ToC に登録する
			if( line.BeginWith( "Table." ) ) {
				TextSpan tmp = line;
				tmp = tmp.Chomp( 6, 0 ).Trim();	// Table. を除去してから Trim
				if( toc.RegisterTable( tmp ) == false ) {
					std::cerr << "ERROR : table '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			if( line.BeginWith( "Figure." ) ) {
				TextSpan tmp = line;
				tmp = tmp.Chomp( 7, 0 ).Trim();	// Figure. を除去してから Trim
				if( toc.RegisterFigure( tmp ) == false ) {
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
			TextSpan tmp = line.TrimTail();
			TextSpan item;
			TextSpan command;
			if( tmp.IsMatch( "<!-- title:", item, " -->" ) ) {
				header.SetTitle( item.Trim() );
				line.Clear();
			}
			if( tmp.IsMatch( "<!-- style:", item, " -->" ) ) {
				header.SetStyleSheet( item.Trim() );
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

	InputFile* InputDataImpl::LoadFileIfNeed( const TextSpan& fileName ) {
		auto itr1 = m_inFiles.begin();
		auto itr2 = m_inFiles.end();
		for( ; itr1 != itr2; ++itr1 ) {
			if( fileName.IsEqual( (*itr1)->GetFileName() ) )
				return *itr1;
		}
		InputFile* pNew = InputFile::LoadInputFile( fileName );
		if( pNew )
			m_inFiles.push_back( pNew );
		return pNew;
	}

	void InputDataImpl::RecursiveLoadFile( InputFileStack& stack, 
										   const TextSpan& fileName ) {
		// 対象ファイルをロード
		InputFile* pInFile = LoadFileIfNeed( fileName );
		if( !pInFile ) {
			AddErrorLine( "Failure loading ", fileName );
			return;
		}
		if( 0 == pInFile->LineSize() )
			return;

		/* 循環インクルードになっていないかチェック */ {
			auto itr1 = stack.begin();
			auto itr2 = stack.end();
			if( std::find( itr1, itr2, pInFile ) != itr2 ) {
				AddErrorLine( "Inclusion loop detected in ", fileName );
//				do {	// 最初のエラーで abort してしまうので無意味
//					--itr2;
//					AddErrorLine( "from ", (*itr2)->GetFileName() );
//				} while( itr1 != itr2 );
				return;
			}
		}
		// ファイルの内容を include を探しつつ行シーケンスに追加
		stack.push_back( pInFile );
		const TextSpan* pTop = pInFile->LineTop();
		const TextSpan* pEnd = pInFile->LineEnd();
		for( ; pTop < pEnd; ++pTop ) {
			TextSpan tmp = pTop->Trim();
			TextSpan fileName;
			if( tmp.IsMatch( "<!-- include:", fileName, "-->" ) == false )
				m_lines.push_back( *pTop );
			else
				RecursiveLoadFile( stack, fileName.Trim() );
		}
		stack.pop_back();
		return;
	}

	void InputDataImpl::AddErrorLine( const char* msg, const TextSpan& fileName ) {
		Buffer buf;
		buf << "<!-- error: " << msg << fileName << " -->";
		m_lines.push_back( buf.GetSpan() );
	}

} // namespace turnup
