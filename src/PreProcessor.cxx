//------------------------------------------------------------------------------
//
// PreProcessor.cxx
//
//------------------------------------------------------------------------------
#include "PreProcessor.hxx"

#include "TextSpan.hxx"

#include <sys/stat.h>	//ToDo : C標準ライブラリの範囲内で実現する必要がある。
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>
#include <iostream>
#include <string.h>
#include <assert.h>

namespace turnup {

	typedef bool UnaryOperator( const TextSpan& operand );
	typedef bool BinaryOperator( const TextSpan& operand1, const TextSpan& operand2 );

	static bool IsVarNameChar( char c );
	static TextSpan GetNextVariableRef( const char* pTop, const char* pEnd );
	static bool IsDefineLine( const TextSpan* pLine, TextSpan& name, TextSpan& value );
	static bool IsConditionTop( const TextSpan* pLine, TextSpan& expression );
	static bool IsNextCondition( const TextSpan* pLine, TextSpan* pExprssion = nullptr );
	static TextSpan* GetEndOfCondClause( TextSpan* pLine, TextSpan* pLineEnd );
	static TextSpan GetNextToken( TextSpan& range );
	static bool CheckExpressionForm( const TextSpan*,
									 uint32_t length, bool bAllowInvert = true );
	static bool EvaluateExpression( TextSpan* pExprTop, TextSpan* pExprEnd );
	static UnaryOperator* GetUnaryOperator( const TextSpan& op );
	static BinaryOperator* GetBinaryOperator( const TextSpan& op );
	static bool Operator4NonZeroString( const TextSpan& v );				// -n STRING
	static bool Operator4ZeroString( const TextSpan& v );					// -z STRING
	static bool Operator4strEQ( const TextSpan& v1, const TextSpan& v2 );	// STR1 == STR2
	static bool Operator4strNE( const TextSpan& v1, const TextSpan& v2 );	// STR1 != STR2
	static bool Operator4intEQ( const TextSpan& v1, const TextSpan& v2 );	// INT1 -eq INT2
	static bool Operator4intNE( const TextSpan& v1, const TextSpan& v2 );	// INT1 -ne INT2
	static bool Operator4intGE( const TextSpan& v1, const TextSpan& v2 );	// INT1 -ge INT2
	static bool Operator4intGT( const TextSpan& v1, const TextSpan& v2 );	// INT1 -gt INT2
	static bool Operator4intLE( const TextSpan& v1, const TextSpan& v2 );	// INT1 -le INT2
	static bool Operator4intLT( const TextSpan& v1, const TextSpan& v2 );	// INT1 -lt INT2
	static bool Operator4fileNT( const TextSpan& v1, const TextSpan& v2 );	// FILE1 -nt FILE2
	static bool Operator4fileOT( const TextSpan& v1, const TextSpan& v2 );	// FILE1 -ot FILE2
	static bool Operator4fileD( const TextSpan& v );						// -d FILE
	static bool Operator4fileE( const TextSpan& v );						// -e FILE
	static bool Operator4fileF( const TextSpan& v );						// -f FILE
	static bool Operator4fileS( const TextSpan& v );						// -s FILE
	static bool ConvertToInteger( const TextSpan& v, int64_t& result );
	static bool CompareAsInteger( const TextSpan& v1,
								  const TextSpan& v2, bool (*cmp)( int64_t, int64_t ) );
	static bool ConvertToFileTime( const TextSpan& v, time_t& result );
	static bool CompareAsFileTime( const TextSpan& v1,
								   const TextSpan& v2, bool (*cmp)( time_t, time_t ) );
	static bool PredicateAsFileInfo( const TextSpan& v, bool (*pred)( mode_t, size_t ) );
	static bool RetrieveFileStatus( const TextSpan& v, struct stat& st );

	//--------------------------------------------------------------------------
	//
	// class PreProcessorImpl
	//
	//--------------------------------------------------------------------------
	class PreProcessorImpl : public PreProcessor {
	public:
		PreProcessorImpl();
		virtual ~PreProcessorImpl();
	public:
		virtual bool Execute( TextSpan* pLineTop, TextSpan* pLineEnd,
							  void (*pCallback)( char*, void* ), void* pOpaque ) override;
		virtual bool RegisterVariable( const TextSpan& name, const TextSpan& value ) override;
	private:
		bool FindVariable( const TextSpan& name, TextSpan& value ) const;
		void ExpandVariables( TextSpan& line,
							  void (*pCallback)( char*, void* ), void* pOpaque );
		char* ExpandVariablesImpl( const char* pTop, 
								   const char* pEnd, TextSpan posRef, uint32_t& length );
		bool SplitExpressionForm( TextSpan (&expr)[4], uint32_t& length );
	private:
		typedef std::pair<TextSpan, TextSpan>	Variable;
		std::vector<Variable>	m_variables;
		std::vector<TextSpan>	m_sequence;	// temporary buffer
	private:
		static bool CompareVariable( const Variable& v1, const Variable& v2 );
	};

	//--------------------------------------------------------------------------
	//
	// implemenation of class PreProcessor
	//
	//--------------------------------------------------------------------------
	PreProcessor::PreProcessor() {
		//do nothing...
	}

	PreProcessor::~PreProcessor() {
		//do nothing...
	}

	PreProcessor* PreProcessor::Create() {
		return new PreProcessorImpl{};
	}

	void PreProcessor::Release( PreProcessor* pPreProcessor ) {
		delete pPreProcessor;
	}


	//--------------------------------------------------------------------------
	//
	// implemenation of class PreProcessorImpl
	//
	//--------------------------------------------------------------------------
	PreProcessorImpl::PreProcessorImpl() : m_variables() {
	}

	PreProcessorImpl::~PreProcessorImpl() {
		m_variables.clear();
	}

	bool PreProcessorImpl::Execute( TextSpan* pLineTop, TextSpan* pLineEnd,
									void (*pCallback)( char*, void* ), void* pOpaque ) {
		//与えられた行シーケンスを反復して処理
		for( TextSpan* pLine = pLineTop; pLine < pLineEnd; ++pLine ) {
			//行内に存在する変数参照を展開
			this->ExpandVariables( *pLine, pCallback, pOpaque );
			//define 行の場合、変数を登録／更新
			TextSpan name, value;
			if( IsDefineLine( pLine, name, value ) ) {
				if( this->RegisterVariable( name, value ) == false ) {
					std::cerr << "ERROR : invalid variable name '" << name << "'." << std::endl;
					return false;
				}
				continue;
			}
			//条件分岐の場合
			TextSpan expr[4];
			uint32_t length = 0;
			bool bSatisfied = false;
			if( IsConditionTop( pLine, expr[0] ) ) {
				do {
					if( SplitExpressionForm( expr, length ) == false ) {
						std::cerr << "ERROR : invalid expression '" << expr[0] << "'." << std::endl;
						return false;
					}
					//対応する　elif/else/endif のいずれかを検索
					TextSpan* pEndOfClause = GetEndOfCondClause( pLine + 1, pLineEnd );
					if( !pEndOfClause ) {
						//見つからない場合はエラー終了
						std::cerr << "ERROR : end of condition missing." << std::endl;
						return false;
					}
					bool result = false;
					//まだ未成立の場合条件式を評価
					if( !bSatisfied )
						result = EvaluateExpression( expr, expr + length );
					//条件式の評価結果が真の場合
					if( result ) {
						bSatisfied = true;
						//現在の clause 内部を再帰呼び出しで処理（内部でエラーなら終了）
						if( this->Execute( pLine + 1, pEndOfClause, pCallback, pOpaque ) == false )
							return false;
					//条件式の評価結果が偽（または成立済み）の場合
					} else {
						//現在の clause 内部を全行クリア
						std::for_each( pLine + 1, pEndOfClause,
									   []( TextSpan& line ) -> void { line.Clear(); } );
					}
					pLine = pEndOfClause;
					this->ExpandVariables( *pLine, pCallback, pOpaque );
				} while( IsNextCondition( pLine, &(expr[0]) ) );
			}
		}
		return true;
	}

	bool PreProcessorImpl::RegisterVariable( const TextSpan& name, const TextSpan& value ) {
		//check name
		if( std::all_of( name.Top(), name.End(), IsVarNameChar ) == false )
			return false;
		Variable tmp{ name, value };
		auto itr = std::lower_bound( m_variables.begin(),
									 m_variables.end(), tmp, CompareVariable );
		if( itr != m_variables.end() && itr->first.IsEqual( name ) )
			itr->second = value;
		else
			m_variables.insert( itr, tmp );
		return true;
	}

	bool PreProcessorImpl::FindVariable( const TextSpan& name, TextSpan& value ) const {
		Variable tmp{ name, TextSpan{} };
		auto itr = std::lower_bound( m_variables.begin(),
									 m_variables.end(), tmp, CompareVariable );
		if( itr != m_variables.end() && itr->first.IsEqual( name ) ) {
			value = itr->second;
			return true;
		}
		return false;
	}

	void PreProcessorImpl::ExpandVariables( TextSpan& line,
											void (*pCallback)( char*, void* ), void* pOpaque ) {
		const char* pTop = line.Top();
		const char* pEnd = line.End();
		//行内に変数参照が存在するかチェック
		TextSpan var = GetNextVariableRef( pTop, pEnd );
		if( var.IsEmpty() == false ) {
			//存在する場合、新しいバッファに展開
			uint32_t length = 0;
			char* newBuf = ExpandVariablesImpl( pTop, pEnd, var, length );
			//現在行の TextSpan を置き換え、バッファをコールバック
			pTop = newBuf;
			pEnd = pTop + length;
			line = TextSpan{ pTop, pEnd };
			pCallback( newBuf, pOpaque );
		}
	}

	char* PreProcessorImpl::ExpandVariablesImpl( const char* pTop,  const char* pEnd,
												 TextSpan posRef, uint32_t& length ) {
		m_sequence.clear();
		do {
			m_sequence.push_back( TextSpan{ pTop, posRef.Top() } ); 
			TextSpan value;
			if( this->FindVariable( posRef.Chomp( 2, 1 ), value ) )
				m_sequence.push_back( value );
			else
				std::cerr << "ERROR : Variable '" << posRef << "' is not found." << std::endl;
			pTop = posRef.End() + 1;
			if( !(pTop <pEnd) )
				break;
			posRef = GetNextVariableRef( pTop, pEnd );
			if( posRef.IsEmpty() ) {
				m_sequence.push_back( TextSpan{ pTop, pEnd } ); 
				pTop = pEnd;
			}
		} while( pTop < pEnd );
		uint32_t len = std::accumulate( m_sequence.begin(), m_sequence.end(), 0,
										[]( uint32_t n, const TextSpan& rng ) -> uint32_t {
											return n + rng.ByteLength();
										} );
		char* pBuf = new char[len+1]; {
			char* p = pBuf;
			for( const auto& rng : m_sequence ) {
				::strncpy( p, rng.Top(), rng.ByteLength() );
				p += rng.ByteLength();
			}
			*p = 0;
		}
		length = len;
		m_sequence.clear();
		return pBuf;
	}

	bool PreProcessorImpl::SplitExpressionForm( TextSpan (&expr)[4], uint32_t& length ) {
		TextSpan target = expr[0];
		m_sequence.clear();
		// target をトークン分割して m_sequence に格納
		while( target.IsEmpty() == false ) {
			m_sequence.push_back( GetNextToken( target ) );
		}
		// check sequence...
		if( CheckExpressionForm( &(m_sequence[0]), m_sequence.size() ) == false )
			return false; 
		length = m_sequence.size();
		std::copy( m_sequence.begin(), m_sequence.end(), &(expr[0]) );
		return true;
	}

	bool PreProcessorImpl::CompareVariable( const Variable& v1, const Variable& v2 ) {
		const auto& name1 = v1.first;
		const auto& name2 = v2.first;
		return std::lexicographical_compare( name1.Top(), name1.End(), name2.Top(), name2.End() );
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static bool IsVarNameChar( char c ) {
		return	('A' <= c && c <= 'Z') || c == '_' ||
			('0' <= c && c <= '9') || ('a' <= c && c <= 'z');
	}

	//行内に最初に登場する ${VAR_NAME} を探し、その位置を返す。ない場合は空 TextSpan を返す
	static TextSpan GetNextVariableRef( const char* pTop, const char* pEnd ) {
		const char* target = "${";
		const char* pCur = pTop;
		while( pCur < pEnd ) {
			auto p1 = std::search( pCur, pEnd, target, target + 2 );
			if( p1 == pEnd )
				return TextSpan{};
			auto p2 = std::find( p1 + 2, pEnd, '}' );
			if( p2 == pEnd )
				return TextSpan{};
			if( std::all_of( p1 + 2, p2, IsVarNameChar ) )
				return TextSpan{ p1, p2 + 1 };
			pCur = p1 + 2;
		}
		return TextSpan{};
	}

	// <!-- define: VAR_NAME = value --> 形式の行かを判定する
	static bool IsDefineLine( const TextSpan* pLine, TextSpan& name, TextSpan& value ) {
		TextSpan line = pLine->Trim();
		TextSpan tmp1;
		TextSpan tmp2;
		if( line.IsMatch( "<!-- define:", tmp1, "=", tmp2, " -->" ) == false )
			return false;
		tmp1 = tmp1.Trim();
		tmp2 = tmp2.Trim();
		auto len = tmp2.ByteLength();
		if( (tmp2[0] == '"' && tmp2[len-1] == '"') ||
			(tmp2[0] == '\'' && tmp2[len-1] == '\'') ) {
			tmp2 = tmp2.Chomp( 1, 1 );
		}
		name  = tmp1;
		value = tmp2;
		return true;
	}

	static bool IsConditionTop( const TextSpan* pLine, TextSpan& expression ) {
		TextSpan expr;
		if( pLine->IsMatch( "<!-- if:", expr, "-->" ) == false )
			return false;
		expression = expr.Trim();
		return true;
	}

	static bool IsNextCondition( const TextSpan* pLine, TextSpan* pExpression ) {
		TextSpan expr;
		TextSpan line = pLine->Trim();
		if( line.IsMatch( "<!-- elif:", expr, "-->" ) ) {
			if( pExpression )
				*pExpression = expr.Trim();
			return true;
		}
		if( line.IsEqual( "<!-- else -->" ) ) {
			// else は（評価されるなら）常に成立しなければならないため、"dummy" という文字列を設定する
			if( pExpression )
				*pExpression = TextSpan{ "dummy" };
			return true;
		}
		return false;
	}

	static TextSpan* GetEndOfCondClause( TextSpan* pLine, TextSpan* pLineEnd ) {
		TextSpan tmp;
		for( ; pLine < pLineEnd; ++pLine ) {
			TextSpan curLine = pLine->Trim();
			// ネストした if に遭遇したらそれに対応する endif を探す
			if( curLine.IsMatch( "<!-- if:", tmp, "-->" ) ) {
				pLine = std::find_if( pLine + 1, pLineEnd,
									  []( const TextSpan& line ) -> bool {
										  return line.Trim().IsEqual( "<!-- endif -->" );
									  } );
				// 見つからなければ現在探している EndOfCondClause も見つからない
				if( pLine == pLineEnd )
					return nullptr;
				continue;
			}
			// endif/elif/else を発見したら終了
			if( curLine.IsEqual( "<!-- endif -->" ) || IsNextCondition( &curLine ) )
				return pLine;
		}
		// ここに到達なら検索失敗 -> nullptr 復帰
		return nullptr;
	}

	static TextSpan GetNextToken( TextSpan& range ) {
		TextSpan tmp = range.Trim();
		const char* pTop = tmp.Top();
		const char* pEnd = tmp.End();
		const char* pDelim = nullptr;
		if( *pTop == '\'' || *pTop == '"' ) {
			do {
				pDelim = std::find( pTop + 1, pEnd, *pTop );
				if( pDelim == pEnd )
					break;
				range = TextSpan{ pDelim + 1, pEnd }.Trim();
				return TextSpan{ pTop + 1, pDelim };
			} while( false );
		}
		pDelim = std::find_if( pTop, pEnd, []( char c ) -> bool {
											   return c == ' ' || c == 0x09; } );
		range = TextSpan{ pDelim, pEnd }.Trim();
		return TextSpan{ pTop, pDelim };
	}

	static bool CheckExpressionForm( const TextSpan* pExpr, 
									 uint32_t length, bool bAllowInvert ) {
		// ４要素を超過するならエラー
		if( 4 < length )
			return false;
		// パラメータ無しなら OK（これは常に偽と判定されることになる）
		if( !length )
			return true;
		// ！で始まる場合
		if( pExpr->IsEqual( "!" ) ) {
			// bAllowInvert == false ならエラー、それ以外は後続の評価結果次第
			if( !bAllowInvert )
				return false;
			else
				return CheckExpressionForm( pExpr + 1, length - 1, false );
		}
		// 残りはパラメータ数で分岐
		switch( length ) {
		case 1: // １要素なら OK（文字列として長さ１以上か否かで判定されることになる）
			return true;
		case 2: // ２要素なら先頭の unary operator をチェック
			if( pExpr[0].ByteLength() != 2 )
				return false;
			if( pExpr[0].Top()[0] != '-' )
				return false;
			if( !::strchr( "nzdefs", pExpr[0].Top()[1] ) )
				return false;
			return true;
		case 3: // ３要素なら２要素目の binary operator をチェック
			switch( pExpr[1].ByteLength() ) {
			case 2: // == or !=
				if( pExpr[1].Top()[1] != '=' )	return false;
				if( pExpr[1].Top()[0] == '=' )	return true;
				if( pExpr[1].Top()[0] == '!' )	return true;
				return false;
			case 3: // -(eq|ne|[lg][te]|[no]t)
				{
					const char* p = pExpr[1].Top();
					if( p[0] != '-' ) 									return false;
					if( p[1] == 'e' && p[2] == 'q' )					return true;
					if( p[1] == 'n' && p[2] == 'e' )					return true;
					if( (p[1] == 'l' || p[1] == 'g') &&
						(p[2] == 't' || p[2] == 'e') )					return true;
					if( (p[1] == 'n' || p[1] == 'o') && p[2] == 't' )	return true;
					return false;
				}
			default:
				return false;
			}
		}
		// ! で始まらない 4 要素に該当するのでエラー
		return false;
	}

	static bool EvaluateExpression( TextSpan* pExprTop, TextSpan* pExprEnd ) {
		//MEMO : CheckExpressionForm で　expression の正当性はチェックできている前提
		// パラメータ無しは偽で確定
		if( pExprTop == pExprEnd )
			return false;
		// ！で始まる場合は後続の評価結果の逆転
		if( pExprTop->IsEqual( "!" ) )
			return !EvaluateExpression( pExprTop + 1, pExprEnd );
		// 残りはパラメータ数で分岐
		switch( pExprEnd - pExprTop ) {
		case 1:
			// １要素なら文字列として長さ１以上か否か
			return (pExprTop->IsEmpty() == false);
		case 2:
			// ２要素なら unary operator として処理
			return GetUnaryOperator( pExprTop[0] )( pExprTop[1] );
		case 3:
			// ３要素なら binary operator として処理
			return GetBinaryOperator( pExprTop[1])( pExprTop[0], pExprTop[2] );
		default:
			assert( false );
		}
		return false;
	}

	static UnaryOperator* GetUnaryOperator( const TextSpan& op ) {
		if( op.IsEqual( "-n" ) ) return Operator4NonZeroString;
		if( op.IsEqual( "-z" ) ) return Operator4ZeroString;
		if( op.IsEqual( "-d" ) ) return Operator4fileD;
		if( op.IsEqual( "-e" ) ) return Operator4fileE;
		if( op.IsEqual( "-f" ) ) return Operator4fileF;
		if( op.IsEqual( "-s" ) ) return Operator4fileS;
		assert( false );
		return nullptr;
	}

	static BinaryOperator* GetBinaryOperator( const TextSpan& op ) {
		if( op.IsEqual(  "==" ) ) return Operator4strEQ;
		if( op.IsEqual(  "!=" ) ) return Operator4strNE;
		if( op.IsEqual( "-eq" ) ) return Operator4intEQ;
		if( op.IsEqual( "-ne" ) ) return Operator4intNE;
		if( op.IsEqual( "-ge" ) ) return Operator4intGE;
		if( op.IsEqual( "-gt" ) ) return Operator4intGT;
		if( op.IsEqual( "-le" ) ) return Operator4intLE;
		if( op.IsEqual( "-lt" ) ) return Operator4intLT;
		if( op.IsEqual( "-nt" ) ) return Operator4fileNT;
		if( op.IsEqual( "-ot" ) ) return Operator4fileOT;
		assert( false );
		return nullptr;
	}

	// -n STRING
	static bool Operator4NonZeroString( const TextSpan& v ) {
		return (v.IsEmpty() == false);
	}
	// -z STRING
	static bool Operator4ZeroString( const TextSpan& v ) {
		return v.IsEmpty();
	}
	// STR1 == STR2
	static bool Operator4strEQ( const TextSpan& v1, const TextSpan& v2 ) {
		return v1.IsEqual( v2 );
	}
	// STR1 != STR2
	static bool Operator4strNE( const TextSpan& v1, const TextSpan& v2 ) {
		return (v1.IsEqual( v2 ) == false);
	}
	// INT1 -eq INT2
	static bool Operator4intEQ( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsInteger( v1, v2,
					[]( int64_t i1, int64_t i2 ) -> bool { return i1 == i2; } );
	}
	// INT1 -ne INT2
	static bool Operator4intNE( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsInteger( v1, v2,
					[]( int64_t i1, int64_t i2 ) -> bool { return i1 != i2; } );
	}
	// INT1 -ge INT2
	static bool Operator4intGE( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsInteger( v1, v2,
					[]( int64_t i1, int64_t i2 ) -> bool { return i1 >= i2; } );
	}
	// INT1 -gt INT2
	static bool Operator4intGT( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsInteger( v1, v2,
					[]( int64_t i1, int64_t i2 ) -> bool { return i1 > i2; } );
	}
	// INT1 -le INT2
	static bool Operator4intLE( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsInteger( v1, v2,
					[]( int64_t i1, int64_t i2 ) -> bool { return i1 <= i2; } );
	}
	// INT1 -lt INT2
	static bool Operator4intLT( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsInteger( v1, v2,
					[]( int64_t i1, int64_t i2 ) -> bool { return i1 < i2; } );
	}
	// FILE1 -nt FILE2
	static bool Operator4fileNT( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsFileTime( v1, v2,
					[]( time_t i1, time_t i2 ) -> bool { return i1 > i2; } );
	}
	// FILE1 -ot FILE2
	static bool Operator4fileOT( const TextSpan& v1, const TextSpan& v2 ) {
		return CompareAsFileTime( v1, v2,
					[]( time_t i1, time_t i2 ) -> bool { return i1 < i2; } );
	}
	// -d FILE
	static bool Operator4fileD( const TextSpan& v ) {
		return PredicateAsFileInfo( v, []( mode_t m, size_t s ) -> bool {
										   (void)s;
										   return (m & S_IFMT) == S_IFDIR;
									   } );
	}
	// -e FILE
	static bool Operator4fileE( const TextSpan& v ) {
		return PredicateAsFileInfo( v, []( mode_t m, size_t s ) -> bool {
										   (void)s;
										   (void)m;
										   return true;
									   } );
	}
	// -f FILE
	static bool Operator4fileF( const TextSpan& v ) {
		return PredicateAsFileInfo( v, []( mode_t m, size_t s ) -> bool {
										   (void)s;
										   return (m & S_IFMT) == S_IFREG;
									   } );
	}
	// -s FILE
	static bool Operator4fileS( const TextSpan& v ) {
		return PredicateAsFileInfo( v, []( mode_t m, size_t s ) -> bool {
										   (void)m;
										   return 0 < s;
									   } );
	}
	static bool ConvertToInteger( const TextSpan& v, int64_t& result ) {
		//雑な実装だけど、int64_t に収まらない数値文字列が指定された場合の挙動は未定義とする
		//仕様なのでこれでよい。
		int64_t spice = 1;
		const char* p1 = v.Top();
		const char* p2 = v.End();
		if( *p1 == '-' ) {
			++p1;
			spice = -1;
		}
		if( std::all_of( p1, p2, []( char c ) -> bool {
									 return '0' <= c && c <= '9';
								 } ) == false ) {
			std::cerr << "ERROR : value '" << v << "' is not integer." << std::endl;
			return false;
		}
		result = 0;
		std::for_each( p1, p2, [&result]( char c ) -> void {
								   result = (result * 10) + (c - '0');
							   } );
		result *= spice;
		return true;
	}
	static bool CompareAsInteger( const TextSpan& v1,
								  const TextSpan& v2, bool (*cmp)( int64_t, int64_t ) ) {
		int64_t i1 = 0;
		int64_t i2 = 0;
		if( !ConvertToInteger( v1, i1 ) ) return false;
		if( !ConvertToInteger( v2, i2 ) ) return false;
		return cmp( i1, i2 );
	}
	static bool ConvertToFileTime( const TextSpan& v, time_t& result ) {
		struct stat st;
		if( RetrieveFileStatus( v, st ) == false ) {
			std::cerr << "ERROR : file '" << v << "' is not exist." << std::endl;
			return false;
		}
		result = st.st_mtime;
		return true;
	}
	static bool CompareAsFileTime( const TextSpan& v1,
								   const TextSpan& v2, bool (*cmp)( time_t, time_t ) ) {
		time_t t1 = 0;
		time_t t2 = 0;
		if( !ConvertToFileTime( v1, t1 ) ) return false;
		if( !ConvertToFileTime( v2, t2 ) ) return false;
		return cmp( t1, t2 );
	}
	static bool PredicateAsFileInfo( const TextSpan& v, bool (*pred)( mode_t, size_t ) ) {
		struct stat st;
		if( RetrieveFileStatus( v, st ) == false )
			return false;
		return pred( st.st_mode, st.st_size );
	}
	static bool RetrieveFileStatus( const TextSpan& v, struct stat& st ) {
		static char s_buf[512];
		static char*  s_pBuf   = nullptr;
		static size_t s_bufLen = 0;
		// v が NULL 終端していれば直接使用
		if( v.End()[0] == 0 ) 
			return (::stat( v.Top(), &st ) == 0);
		uint32_t len = v.ByteLength();
		// v が静的バッファに収まるならそれを使用
		if( v.ByteLength() < sizeof(s_buf) ) {
			::strncpy( s_buf, v.Top(), len );
			s_buf[len] = 0;
			return (::stat( s_buf, &st ) == 0);
		}
		// v が確保済みのバッファに収まらない場合はそれを拡張
		if( s_bufLen <= len ) {
			delete[] s_pBuf;
			s_bufLen = len + 1;
			s_pBuf = new char[s_bufLen];
		}
		::strncpy( s_pBuf, v.Top(), len );
		s_pBuf[len] = 0;
		return (::stat( s_pBuf, &st ) == 0);
	}

} // namespace turnup

