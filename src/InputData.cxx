//------------------------------------------------------------------------------
//
// InputData.cxx
//
//------------------------------------------------------------------------------
#include "InputData.hxx"

#include "TextMaker.hxx"
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
#include "Snippet.hxx"

#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <string.h>
#include <assert.h>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// class FileFinder
	//
	//--------------------------------------------------------------------------
	class FileFinder {
	public:
		FileFinder( const TextSpan* pIncPathTop, const TextSpan* pIncPathEnd );
		~FileFinder();
	public:
		TextSpan FindFile( const TextSpan& fileName, const TextSpan& curFileName );
	private:
		const TextSpan* m_pPathTop;
		const TextSpan* m_pPathEnd;
	};

	//--------------------------------------------------------------------------
	//
	// class InputDataImpl
	//
	//--------------------------------------------------------------------------
	class InputDataImpl : public InputData {
	private:
		typedef std::vector<InputFile*> InputFileStack;
	public:
		InputDataImpl( const TextSpan& fileName,
					   const TextSpan* pIncPathTop, const TextSpan* pIncPathEnd );
		virtual ~InputDataImpl();
	public:
		virtual uint32_t Size() const override;
		virtual const TextSpan* Begin() const override;
		virtual const TextSpan* End() const override;
		virtual bool PreProcess( PreProcessor* pPreProsessor ) override;
		virtual void PreScan( DocumentInfo& docInfo ) override;
	private:
		InputFile* LoadFileIfNeed( const TextSpan& fileName, const TextSpan& curFileName );
		void RecursiveLoadFile( InputFileStack& stack,
								const TextSpan& fileName, const TextSpan& curFileName );
		void AddErrorLine( std::vector<TextSpan>& lines,
						   const char* msg, const TextSpan& fileName );
	private:
		std::vector<InputFile*>	m_inFiles;
		std::vector<TextSpan>	m_lines;
		FileFinder				m_fileFinder;
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

	InputData* InputData::Create( const TextSpan& fileName,
								  const TextSpan* pIncPathTop, const TextSpan* pIncPathEnd ) {
		return new InputDataImpl{ fileName, pIncPathTop, pIncPathEnd };
	}

	void InputData::Release( InputData* pInputData ) {
		delete pInputData;
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class FileFinder
	//
	//--------------------------------------------------------------------------
	FileFinder::FileFinder( const TextSpan* pIncPathTop,
							const TextSpan* pIncPathEnd ) : m_pPathTop( pIncPathTop ),
															m_pPathEnd( pIncPathEnd ) {
	}

	FileFinder::~FileFinder() {
	}

	TextSpan FileFinder::FindFile( const TextSpan& fileName,
								   const TextSpan& curFileName ) {
		// ??????????????????????????????????????????
		if( File::IsFullPath( fileName ) ) {
			// ?????????????????????????????????????????????????????????????????? TextSpan ?????????
			if( File::IsExist( fileName ) )
				return fileName;
			else
				return TextSpan{};

		// ?????????????????????
		} else {
			TextSpan path{};
			TextSpan curPath = File::GetPath( curFileName );
			// ??????????????????????????????????????????????????????
			if( File::IsExist( curPath, fileName, &path ) ) {
			#ifndef NDEBUG
				std::cerr << "NOTE : file '" << fileName << "'"
						  << " found in " << path << "." << std::endl;
			#endif
				return path;
			}
			// ????????????????????? -I ????????????????????????????????????
			for( const TextSpan* pPath = m_pPathTop; pPath != m_pPathEnd; ++pPath ) {
				if( File::IsExist( *pPath, fileName, &path ) ) {
				#ifndef NDEBUG
					std::cerr << "NOTE : file '" << fileName << "'"
							  << " found in " << *pPath << "." << std::endl;
				#endif
					return path;
				}
			}
		}
		return TextSpan{};
	}

	//--------------------------------------------------------------------------
	//
	// implementation of class InputDataImpl
	//
	//--------------------------------------------------------------------------
	InputDataImpl::InputDataImpl( const TextSpan& fileName,
								  const TextSpan* pIncPathTop,
								  const TextSpan* pIncPathEnd ) : InputData(),
																  m_inFiles(),
																  m_lines(),
																  m_fileFinder( pIncPathTop,
																				pIncPathEnd ) {
		m_lines.reserve( 1000 );	//ToDo : ok?
		InputFileStack stack;
		RecursiveLoadFile( stack, fileName, TextSpan{} );
		Snippet::Expand( m_lines );
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
			//????????????????????? ToC ???????????????
			uint32_t lv = line.CountTopOf('#');
			if( 0 < lv && lv <= 6 && line[lv] == ' ' ) {
				TextSpan tmp = line.TrimTail();	// ?????????????????????????????????
				tmp.Chomp( lv + 1, 0 );
				if( toc.RegisterHeader( lv, tmp ) == false ) {
					std::cerr << "ERROR : header '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			//?????????????????????????????? ToC ???????????????
			if( line.BeginWith( "Table." ) ) {
				TextSpan tmp = line;
				tmp = tmp.Chomp( 6, 0 ).Trim();	// Table. ????????????????????? Trim
				if( toc.RegisterTable( tmp ) == false ) {
					std::cerr << "ERROR : table '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			if( line.BeginWith( "Figure." ) ) {
				TextSpan tmp = line;
				tmp = tmp.Chomp( 7, 0 ).Trim();	// Figure. ????????????????????? Trim
				if( toc.RegisterFigure( tmp ) == false ) {
					std::cerr << "ERROR : figure '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			//?????????????????????
			if( line.BeginWith( "*[" ) ) {
				const char* pTerm1;
				const char* pTerm2;
				if( IsTermDefine( line, pTerm1, pTerm2 ) ) {
					Utilities::Trim( pTerm1, pTerm2 );
					if( glossary.RegisterTerm( pTerm1, pTerm2 ) == false )
						std::cerr << "ERROR : link keyword '"
								  << TextSpan{ pTerm1, pTerm2 } << "' is duplicated." << std::endl;
				}
				continue;
			}
			//?????????????????????
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
			if( tmp.IsMatch( "<!-- anchor:", item, " -->" ) ) {
				item = item.Trim();
				if( toc.RegisterAnchor( item ) == false ) {
					std::cerr << "ERROR : anchor '";
					std::cerr.write( item.Top(), item.ByteLength() );
					std::cerr << "' is duplicated." << std::endl;
				}
				continue;
			}
			if( tmp.IsMatch( "<!-- autolink:", item, " -->" ) ) {
				item = item.Trim();
				TextSpan word;
				TextSpan url;
				if( item.IsMatch( "[", word, "](", url, ")" ) == false ) {
					std::cerr << "ERROR : invalid autolink format '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "'." << std::endl;
				} else {
					if( word.IsEmpty() )
						std::cerr << "ERROR : link keyword is empty." << std::endl;
					else if( glossary.RegisterAutoLink( word.Top(), word.End(),
														url.Top(), url.End() ) == false )
						std::cerr << "ERROR : link keyword '"
								  << word << "' is duplicated." << std::endl;
				}
				line.Clear();
			}
			if( tmp.IsMatch( "<!-- toc-link:", item, " -->" ) ) {
				//ToDo : 1Grw5djeUn8 : ??????????????????????????????????????????????????????????????????????????????????????????
				item = item.Trim();
				TextSpan cur = item.CutNextToken();
				bool     bTop;
				if( cur.IsEqual( "top" ) )
					bTop = true;
				else if( cur.IsEqual( "bottom" ) )
					bTop = false;
				else {
					std::cerr << "ERROR : invalid toc-link format '";
					std::cerr.write( tmp.Top(), tmp.ByteLength() );
					std::cerr << "'." << std::endl;
					continue;
				}
				item = item.Trim();
				if( item.IsQuoted() )
					item.Chomp( 1, 1 );
				bool ret = toc.RegisterLinkButton( bTop, item );
				if( !ret )
					std::cerr << "WARNING : toc-link directive duplicated." << std::endl;
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

	InputFile* InputDataImpl::LoadFileIfNeed( const TextSpan& fileName,
											  const TextSpan& curFileName ) {
		InputFile* pNew = nullptr;
		// curFileName ??????????????????????????????????????? ??? ?????????????????????
		if( curFileName.IsEmpty() ) {
			pNew = InputFile::LoadInputFile( fileName );

		// ????????????????????????FileFinder ?????????????????????
		} else {
			// ????????? FileFinder ?????????????????????
			TextSpan foundPath = m_fileFinder.FindFile( fileName, curFileName );
			if( foundPath.IsEmpty() == false ) {
				// ?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
				auto itr1 = m_inFiles.begin();
				auto itr2 = m_inFiles.end();
				for( ; itr1 != itr2; ++itr1 ) {
					if( fileName.IsEqual( (*itr1)->GetFileName() ) )
						return *itr1;
				}
				// ?????????????????????????????????				
				pNew = InputFile::LoadInputFile( foundPath );
			}
		}
		// ??????????????????????????????????????????????????????
		if( pNew )
			m_inFiles.push_back( pNew );
		return pNew;
	}

	void InputDataImpl::RecursiveLoadFile( InputFileStack& stack,
										   const TextSpan& fileName,
										   const TextSpan& curFileName ) {
		// ??????????????????????????????
		InputFile* pInFile = LoadFileIfNeed( fileName, curFileName );
		if( !pInFile ) {
			AddErrorLine( m_lines, "Failure loading ", fileName );
			return;
		}
		if( 0 == pInFile->LineSize() )
			return;

		/* ???????????????????????????????????????????????????????????? */ {
			auto itr1 = stack.begin();
			auto itr2 = stack.end();
			if( std::find( itr1, itr2, pInFile ) != itr2 ) {
				AddErrorLine( m_lines, "Inclusion loop detected in ", fileName );
//				do {	// ????????????????????? abort ??????????????????????????????
//					--itr2;
//					AddErrorLine( m_lines, "from ", (*itr2)->GetFileName() );
//				} while( itr1 != itr2 );
				return;
			}
		}
		// ???????????????????????? include ??????????????????????????????????????????
		stack.push_back( pInFile );
		const TextSpan* pTop = pInFile->LineTop();
		const TextSpan* pEnd = pInFile->LineEnd();
		for( ; pTop < pEnd; ++pTop ) {
			TextSpan tmp = pTop->Trim();
			TextSpan incFileName;
			if( tmp.IsMatch( "<!-- include:", incFileName, "-->" ) == false )
				m_lines.push_back( *pTop );
			else
				RecursiveLoadFile( stack, incFileName.Trim(), pInFile->GetFileName() );
		}
		stack.pop_back();
		return;
	}

	void InputDataImpl::AddErrorLine( std::vector<TextSpan>& lines,
									  const char* msg, const TextSpan& fileName ) {
		TextMaker tm;
		tm << "<!-- error: " << msg << fileName << " -->";
		lines.push_back( tm.GetSpan() );
	}

} // namespace turnup
