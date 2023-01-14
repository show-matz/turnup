//------------------------------------------------------------------------------
//
// Operators.cxx
//
//------------------------------------------------------------------------------
#include "Operators.hxx"

#include "Operator4Anchor.hxx"
#include "Operator4Comment.hxx"
#include "Operator4Details.hxx"
#include "Operator4EmbedFootnotes.hxx"
#include "Operator4EmbedPageBreak.hxx"
#include "Operator4EmbedIndex.hxx"
#include "Operator4EmbedTOC.hxx"
#include "Operator4EmptyLine.hxx"
#include "Operator4Error.hxx"
#include "Operator4FigureAndTable.hxx"
#include "Operator4Header.hxx"
#include "Operator4HorizontalLine.hxx"
#include "Operator4Image.hxx"
#include "Operator4List.hxx"
#include "Operator4MathJaxLine.hxx"
#include "Operator4Paragraph.hxx"
#include "Operator4PreBlock.hxx"
#include "Operator4Quote.hxx"
#include "Operator4RawHTML.hxx"
#include "Operator4StylePalette.hxx"
#include "Operator4StyleStack.hxx"
#include "Operator4Table.hxx"
#include "Operator4TermDefine.hxx"
#include "Utilities.hxx"

#include <stdint.h>
//#include <algorithm>

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class Operators
	//
	//--------------------------------------------------------------------------
	static OperatorType* const s_operators[] = {
		Operator4EmptyLine,
		Operator4Header,
		Operator4RawHTML,
		Operator4List,
		Operator4PreBlock,
		Operator4Table,
		Operator4Quote,
		Operator4Image,
		Operator4TermDefine,
		Operator4FigureAndTable,
		Operator4Anchor,
		Operator4Details,
		Operator4EmbedTOC_X,
		Operator4EmbedTOC,
		Operator4EmbedSubTOC,
		Operator4EmbedTableList,
		Operator4EmbedFigureList,
		Operator4EmbedIndex_X,
		Operator4EmbedIndex,
		Operator4EmbedFootnotes,
		Operator4EmbedPageBreak,
		Operator4StyleStack,
		Operator4StylePalette,
		Operator4MathJaxLine,
		Operator4Error,
		Operator4Comment,
		Operator4HorizontalLine,
		Operator4Paragraph
	};
   
	//--------------------------------------------------------------------------
	//
	// implementation of class Operators
	//
	//--------------------------------------------------------------------------
	Operators::Operators() {
	}

	Operators::~Operators() {
	}

	const TextSpan* Operators::OperateLines( const TextSpan* pTop,
											 const TextSpan* pEnd, DocumentInfo& docInfo ) {
		for( uint32_t i = 0; i < count_of(s_operators); ++i ) {
			const TextSpan* pRet = s_operators[i]( pTop, pEnd, docInfo );
			if( !pRet || pTop < pRet )	// null means 'abort by error'
				return pRet;
		}
		//ToDo : implement... output error message ?
		return pEnd;
	}

	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------

} // namespace turnup
