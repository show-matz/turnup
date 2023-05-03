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
		typedef bool MuteBlockEndChecker( const TextSpan& );
		static MuteBlockEndChecker* IsMuteBlockStart( const TextSpan& line );
		static bool IsPreBlockFinished1( const TextSpan& line );
		static bool IsPreBlockFinished2( const TextSpan& line );
		static bool IsCommentBlockFinished( const TextSpan& line );
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
		// 指定ファイルがフルパスの場合
		if( File::IsFullPath( fileName ) ) {
			// 実在ファイルならそのまま返却し、それ以外は空 TextSpan を返却
			if( File::IsExist( fileName ) )
				return fileName;
			else
				return TextSpan{};

		// 上記以外の場合
		} else {
			TextSpan path{};
			TextSpan curPath = File::GetPath( curFileName );
			// まずは処理中ファイルのあるパスで検索
			if( File::IsExist( curPath, fileName, &path ) ) {
			#ifndef NDEBUG
				std::cerr << "NOTE : file '" << fileName << "'"
						  << " found in " << path << "." << std::endl;
			#endif
				return path;
			}
			// 上記でダメなら -I 指定の検索パスを順に処理
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
		auto& cfg      = docInfo.Get<Config>();

		auto itr1 = m_lines.begin();
		auto itr2 = m_lines.end();

		MuteBlockEndChecker* chkMuteEnd = nullptr;

		for( ; itr1 != itr2; ++itr1 ) {
			TextSpan& line = *itr1;

			if( !chkMuteEnd ) {
				if( auto tmp = IsMuteBlockStart( line ) ) {
					chkMuteEnd = tmp;
					continue;
				}
			} else {
				if( chkMuteEnd( line ) == true )
					chkMuteEnd = nullptr;
				continue;
			}

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
			TextSpan tmp = line.TrimTail();
			//MathJax 使用行の場合
			if( tmp.IsEqual( "```math" ) || tmp.IsEqual( "~~~math" ) ||
					(tmp.TrimHead().BeginWith( "$$" ) && tmp.TrimHead().EndWith( "$$" )) ) {
				cfg.bUseMathJax = true;
				continue;
			}
			//コメントの場合
			if( tmp.BeginWith( "<!-- " ) && tmp.EndWith( " -->" ) ) {
				TextSpan item;
				TextSpan command;
				if( tmp.IsMatch( "<!-- title:", item, " -->" ) ) {
					header.SetTitle( item.Trim() );
					line.Clear();
					continue;
				}
				if( tmp.IsMatch( "<!-- style:", item, " -->" ) ) {
					header.SetStyleSheet( item.Trim() );
					line.Clear();
					continue;
				}
				if( tmp.IsMatch( "<!-- filter:", item, "=", command, " -->" ) ) {
					filters.RegistExternal( item.Trim(), command.Trim() );
					line.Clear();
					continue;
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
					continue;
				}
				if( tmp.IsMatch( "<!-- toc-link:", item, " -->" ) ) {
					//ToDo : 1Grw5djeUn8 : パラメータが３つ以上存在する場合をエラーとして検出できてない
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
					continue;
				}
				if( tmp.IsMatch( "<!-- config:", item, " -->" ) ) {
					if( item.IsEqual( "term-link-in-header" ) ) {
						cfg.bTermLinkInHeader = true;
					} else if( item.IsEqual( "embed-stylesheet" ) ) {
						cfg.bEmbedStyleSheet = true;
					} else if( item.IsEqual( "write-comment" ) ) {
						cfg.bWriteComment = true;
					} else if( item.IsEqual( "use-mathjax" ) ) {
						cfg.bUseMathJax = true;
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
					continue;
				}
			}
			/* インラインアンカーの使用をチェックする */ {
				auto top = tmp.Top();
				auto end = tmp.End();
				while( top < end ) {
					auto i1 = std::find( top, end, '#' );
					if( i1 == end ) break;
					if( (i1[1] != '(' && i1[1] != '{') || i1[1] != i1[2] ) {
						top = i1 + 1;
						continue;
					}	
					char par[2];
					par[0] = par [1] = (i1[1] == '(') ? ')' : '}';
					auto i2 = std::search( i1 + 3, end, par, par + 2 );
					if( i2 == end )  {
						top = i1 + 3;
						continue;
					}
					TextSpan name{ i1 + 3, i2 };
					if( toc.RegisterAnchor( name ) == false ) {
						std::cerr << "ERROR : anchor '";
						std::cerr.write( name.Top(), name.ByteLength() );
						std::cerr << "' is duplicated." << std::endl;
					}
					top = i2 + 2;
				}
			}
			//最後に $..$ 形式の MathJax 利用をチェックする
			do {
				auto top = tmp.Top();
				auto end = tmp.End();
				auto i1 = std::find( top, end, '$' );
				if( i1 == end ) break;
				if( !( i1 == top || IsSpaceBackward( i1 - 1 ) ) ) break;
				auto i2 = std::find( i1 + 1, end, '$' );
				if( i2 == end ) break;
				if( !( (i2 + 1) == end || IsSpaceForward( i2 + 1 ) ) ) break;
				cfg.bUseMathJax = true;
			} while( false );
		}
	}

	InputFile* InputDataImpl::LoadFileIfNeed( const TextSpan& fileName,
											  const TextSpan& curFileName ) {
		InputFile* pNew = nullptr;
		// curFileName が空なら最初の入力ファイル ⇒ そのままロード
		if( curFileName.IsEmpty() ) {
			pNew = InputFile::LoadInputFile( fileName );

		// 上記以外の場合、FileFinder に探してもらう
		} else {
			// まずは FileFinder に探してもらう
			TextSpan foundPath = m_fileFinder.FindFile( fileName, curFileName );
			if( foundPath.IsEmpty() == false ) {
				// みつかった場合、ロード済みファイルキャッシュにないか探し、見つかればそれを返す
				auto itr1 = m_inFiles.begin();
				auto itr2 = m_inFiles.end();
				for( ; itr1 != itr2; ++itr1 ) {
					if( fileName.IsEqual( (*itr1)->GetFileName() ) )
						return *itr1;
				}
				// みつからなければロード				
				pNew = InputFile::LoadInputFile( foundPath );
			}
		}
		// 新規ロードしていればキャッシュに追加
		if( pNew )
			m_inFiles.push_back( pNew );
		return pNew;
	}

	void InputDataImpl::RecursiveLoadFile( InputFileStack& stack,
										   const TextSpan& fileName,
										   const TextSpan& curFileName ) {
		// 対象ファイルをロード
		InputFile* pInFile = LoadFileIfNeed( fileName, curFileName );
		if( !pInFile ) {
			AddErrorLine( m_lines, "Failure loading ", fileName );
			return;
		}
		if( 0 == pInFile->LineSize() )
			return;

		/* 循環インクルードになっていないかチェック */ {
			auto itr1 = stack.begin();
			auto itr2 = stack.end();
			if( std::find( itr1, itr2, pInFile ) != itr2 ) {
				AddErrorLine( m_lines, "Inclusion loop detected in ", fileName );
//				do {	// 最初のエラーで abort してしまうので無意味
//					--itr2;
//					AddErrorLine( m_lines, "from ", (*itr2)->GetFileName() );
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

	InputDataImpl::MuteBlockEndChecker* InputDataImpl::IsMuteBlockStart( const TextSpan& line ) {
		if( line.BeginWith( "```" ) )
			return IsPreBlockFinished1;
		if( line.BeginWith( "~~~" ) )
			return IsPreBlockFinished2;
		TextSpan tmp = line.Trim();
		if( tmp.BeginWith( "<!--" ) && !(tmp.EndWith( "-->" )) )
			return IsCommentBlockFinished;
		return nullptr;
	}

	bool InputDataImpl::IsPreBlockFinished1( const TextSpan& line ) {
		return line.BeginWith( "```" );
	}

	bool InputDataImpl::IsPreBlockFinished2( const TextSpan& line ) {
		return line.BeginWith( "~~~" );
	}

	bool InputDataImpl::IsCommentBlockFinished( const TextSpan& line ) {
		return line.Trim().EndWith( "-->" );
	}

} // namespace turnup
