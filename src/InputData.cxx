//------------------------------------------------------------------------------
//
// InputData.cxx
//
//------------------------------------------------------------------------------
#include "InputData.hxx"

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
	public:
		bool Loaded() const;
	private:
		void Cleanup();
		InputFile* LoadFileIfNeed( const TextSpan& fileName );
		bool RecursiveLoadFile( InputFileStack& stack, const TextSpan& fileName );
	private:
		std::vector<InputFile*>	m_inFiles;
		std::vector<TextSpan>	m_lines;
		std::vector<char*>		m_buffers;
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
		InputDataImpl* pImpl = new InputDataImpl{ fileName };
		if( pImpl->Loaded() == false ) {
			Release( pImpl );
			pImpl = nullptr;
		}
		return pImpl;
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
															   m_lines(),
															   m_buffers() {
		m_lines.reserve( 1000 );	//ToDo : ok?
		InputFileStack stack;
		if( RecursiveLoadFile( stack, fileName ) == false )
			this->Cleanup();
	}

	InputDataImpl::~InputDataImpl() {
		this->Cleanup();
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
		auto callback = []( char* pBuffer, void* pOpaque ) -> void {
			auto pContainer = reinterpret_cast<std::vector<char*>*>( pOpaque );
			pContainer->push_back( pBuffer );
		};
		return pPreProsessor->Execute( pTop, pEnd, callback, &m_buffers );
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

	bool InputDataImpl::Loaded() const {
		return (0 < m_inFiles.size()) && (0 < m_lines.size());
	}

	void InputDataImpl::Cleanup() {
		m_lines.clear();
		auto itr1 = m_inFiles.begin();
		auto itr2 = m_inFiles.begin();
		for( ; itr1 != itr2; ++itr1 ) {
			InputFile::ReleaseInputFile( *itr1 );
			*itr1 = nullptr;
		}
		m_inFiles.clear();

		/* clear preprocessed line buffes */ {
			auto itr1 = m_buffers.begin();
			auto itr2 = m_buffers.end();
			for( ; itr1 != itr2; ++itr1 ) {
				char* p = *itr1;
				delete[] p;
				*itr1 = nullptr;
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

	bool InputDataImpl::RecursiveLoadFile( InputFileStack& stack, 
										   const TextSpan& fileName ) {

		InputFile* pInFile = LoadFileIfNeed( fileName );
		if( !pInFile ) {
			std::cerr << "ERROR : Failure loading '";
			std::cerr.write( fileName.Top(), fileName.ByteLength() );
			std::cerr << "'." << std::endl;
			return false;
		}
		if( 0 == pInFile->LineSize() )
			return true;

		{ // Check inclusion loop.
			auto itr1 = stack.begin();
			auto itr2 = stack.end();
			if( std::find( itr1, itr2, pInFile ) != itr2 ) {
				std::cerr << "ERROR : Inclusion loop detected." << std::endl;
				for( ; itr1 != itr2; ++itr1 ) {
					const TextSpan& file = (*itr1)->GetFileName();
					std::cerr << "        --> ";
					std::cerr.write( file.Top(), file.ByteLength() );
					std::cerr << std::endl;
				}
				std::cerr << "        --> ";
				std::cerr.write( fileName.Top(), fileName.ByteLength() );
				std::cerr << std::endl;
				return false;
			}
		}
		bool result = true;
		stack.push_back( pInFile );
		const TextSpan* pTop = pInFile->LineTop();
		const TextSpan* pEnd = pInFile->LineEnd();
		for( ; pTop < pEnd; ++pTop ) {
			TextSpan tmp = pTop->Trim();
			TextSpan fileName;
			if( tmp.IsMatch( "<!-- include:", fileName, "-->" ) == false )
				m_lines.push_back( *pTop );
			else {
				fileName = fileName.Trim();
				if( RecursiveLoadFile( stack, fileName ) == false ) {
					result = false;
					break;
				}
			}
		}
		stack.pop_back();
		return result;
	}

} // namespace turnup
