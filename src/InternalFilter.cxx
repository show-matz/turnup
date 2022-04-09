//------------------------------------------------------------------------------
//
// InternalFilter.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter.hxx"

#include "InternalFilter4CandCxx.hxx"
#include "InternalFilter4CommonLISP.hxx"
#include "InternalFilter4Diff.hxx"
#include "InternalFilter4Shell.hxx"
#include "InternalFilter4Raw.hxx"
#include "InternalFilter4Table.hxx"

#include "TextSpan.hxx"

#include <ostream>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class utility InternalFilter
	//
	//--------------------------------------------------------------------------
	InternalFilterFunc* InternalFilter::FindFilter( const TextSpan& type ) {
		if( type.IsEqual( "raw" ) )		return InternalFilter4Raw;
		if( type.IsEqual( "table" ) )	return InternalFilter4Table;
		if( type.IsEqual( "C" ) ||
			type.IsEqual( "c" ) )		return InternalFilter4C;
		if( type.IsEqual( "C++" ) ||
			type.IsEqual( "c++" ) )		return InternalFilter4Cxx;
		if( type.IsEqual( "diff" ) )	return InternalFilter4Diff;
		if( type.IsEqual( "sh" ) )		return InternalFilter4Shell;
		if( type.IsEqual( "lisp" ) )	return InternalFilter4CommonLISP;
		return nullptr;
	}

	void InternalFilter::ExecRecursive( std::ostream& os,
										const TextSpan& span,
										const RangeFinderUnit* pUnit ) {
		TextSpan result{};
		const char* pClassName;
		while( true ) { 
			//現在の finder 関数と target を取得
			RangeFinder* finder = pUnit->finder;
			const char*  target = pUnit->target;
			//終端に達していれば span にはハイライト対象が無いのでそのまま出力して終了
			if( !finder && !target ) {
				span.WriteSimple( os );
				return;
			}
			//finder 関数を実行し、true 復帰なら break
			pClassName = pUnit->className;
			if( finder( span, target, result, pClassName ) == true )
				break;
			//上記以外の場合は次の unit に移動してループ続行
			++pUnit;
		}
		//発見した範囲の手前までを再帰的に処理（今見つけたものは無いとわかっているのでその次から）
		ExecRecursive( os, TextSpan{ span.Top(), result.Top() }, pUnit + 1 );

		//発見した範囲を span tag 付きで出力
		os << "<span class='" << pClassName << "'>";	//MEMO : ignore StyleStack.
		result.WriteSimple( os );
		os << "</span>";
		
		//発見した範囲よりも後ろを再帰的に処理（今見つけたものがあるかもなのでおなじ unit から）
		ExecRecursive( os, TextSpan{ result.End(), span.End() }, pUnit );
	}

} // namespace turnup
