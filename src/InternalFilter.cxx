//------------------------------------------------------------------------------
//
// InternalFilter.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter.hxx"

#include "InternalFilter4CandCxx.hxx"
#include "InternalFilter4Diff.hxx"

#include "TextSpan.hxx"

#include <ostream>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class utility InternalFilter
	//
	//--------------------------------------------------------------------------
	InternalFilterFunc* InternalFilter::FindFilter( const TextSpan& type ) {
		if( type.IsEqual( "C" ) ||
			type.IsEqual( "c" ) )		return InternalFilter4C;
		if( type.IsEqual( "C++" ) ||
			type.IsEqual( "c++" ) )		return InternalFilter4Cxx;
		if( type.IsEqual( "diff" ) )	return InternalFilter4Diff;
		return nullptr;
	}

	void InternalFilter::ExecRecursive( std::ostream& os,
										const TextSpan& span,
										const RangeFinderUnit* pUnit ) {
		TextSpan result{};
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
			if( finder( span, target, result ) == true )
				break;
			//上記以外の場合は次の unit に移動してループ続行
			++pUnit;
		}
		//発見した範囲の手前までを再帰的に処理（今見つけたものは無いとわかっているのでその次から）
		ExecRecursive( os, TextSpan{ span.Top(), result.Top() }, pUnit + 1 );

		//発見した範囲を span tag 付きで出力
		os << "<span class='" << pUnit->className << "'>";
		result.WriteSimple( os );
		os << "</span>";
		
		//発見した範囲よりも後ろを再帰的に処理（今見つけたものがあるかもなのでおなじ unit から）
		ExecRecursive( os, TextSpan{ result.End(), span.End() }, pUnit );
	}

} // namespace turnup
