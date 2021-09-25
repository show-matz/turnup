//------------------------------------------------------------------------------
//
// PreProcessor.cxx
//
//------------------------------------------------------------------------------
#include "PreProcessor.hxx"

#include "TextSpan.hxx"

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>
#include <iostream>
#include <string.h>

namespace turnup {

	static bool IsVarNameChar( char c );
	static TextSpan GetNextVariableRef( const char* pTop, const char* pEnd );
	static bool IsDefineLine( const TextSpan* pLine, TextSpan& name, TextSpan& value );

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
	private:
		void RegisterVariable( const TextSpan& name, const TextSpan& value );
		bool FindVariable( const TextSpan& name, TextSpan& value ) const;
		char* ExpandVariables( const char* pTop, 
							   const char* pEnd, TextSpan posRef, uint32_t& length );
	private:
		typedef std::pair<TextSpan, TextSpan>	Variable;
		std::vector<Variable>	m_variables;
		std::vector<TextSpan>	m_sequence;
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
		//ToDo : eiRfJCrt9ae : システム変数はコンストラクタに渡して登録、だろうな。 ⇒ 将来やる
	}

	PreProcessorImpl::~PreProcessorImpl() {
		m_variables.clear();
	}

	bool PreProcessorImpl::Execute( TextSpan* pLineTop, TextSpan* pLineEnd,
									void (*pCallback)( char*, void* ), void* pOpaque ) {
		//与えられた行シーケンスを反復して処理
		for( TextSpan* pLine = pLineTop; pLine < pLineEnd; ++pLine ) {
			const char* pTop = pLine->Top();
			const char* pEnd = pLine->End();
			//行内に変数参照が存在するかチェック
			TextSpan var = GetNextVariableRef( pTop, pEnd );
			if( var.IsEmpty() == false ) {
				//存在する場合、新しいバッファに展開
				uint32_t length = 0;
				char* newBuf = ExpandVariables( pTop, pEnd, var, length );
				//現在行の TextSpan を置き換え、バッファをコールバック
				pTop = newBuf;
				pEnd = pTop + length;
				*pLine = TextSpan{ pTop, pEnd };
				pCallback( newBuf, pOpaque );
			}
			//define 行の場合、変数を登録／更新
			TextSpan name, value;
			if( IsDefineLine( pLine, name, value ) ) {
				if( std::all_of( name.Top(), name.End(), IsVarNameChar ) == false ) {
					std::cerr << "ERROR : invalid variable name '" << name << "'." << std::endl;
					return false;
				}
				this->RegisterVariable( name, value );
			}
			//ToDo : eiRfJCrt9ae : 条件分岐を処理する必要がある... ⇒ 将来やる
		}
		return true;
	}

	void PreProcessorImpl::RegisterVariable( const TextSpan& name, const TextSpan& value ) {
		Variable tmp{ name, value };
		auto itr = std::lower_bound( m_variables.begin(),
									 m_variables.end(), tmp, CompareVariable );
		if( itr != m_variables.end() && itr->first.IsEqual( name ) )
			itr->second = value;
		else
			m_variables.insert( itr, tmp );
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

	char* PreProcessorImpl::ExpandVariables( const char* pTop,  const char* pEnd,
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
		return pBuf;
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

} // namespace turnup
