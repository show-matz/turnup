//------------------------------------------------------------------------------
//
// Operators.cxx
//
//------------------------------------------------------------------------------
#include "Operators.hxx"

#include "Operator4Comment.hxx"
#include "Operator4EmbedFootnotes.hxx"
#include "Operator4EmbedPageBreak.hxx"
#include "Operator4EmbedTOC.hxx"
#include "Operator4EmptyLine.hxx"
#include "Operator4FigureAndTable.hxx"
#include "Operator4Header.hxx"
#include "Operator4HorizontalLine.hxx"
#include "Operator4Image.hxx"
#include "Operator4NormalList.hxx"
#include "Operator4NumberedList.hxx"
#include "Operator4Paragraph.hxx"
#include "Operator4PreBlock.hxx"
#include "Operator4Quote.hxx"
#include "Operator4RawHTML.hxx"
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
		Operator4NormalList,
		Operator4NumberedList,
		Operator4PreBlock,
		Operator4Table,
		Operator4Quote,
		Operator4Image,
		Operator4TermDefine,
		Operator4FigureAndTable,
		Operator4EmbedTOC,
		Operator4EmbedTableList,
		Operator4EmbedFigureList,
		Operator4EmbedFootnotes,
		Operator4EmbedPageBreak,
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
			if( !!pRet )
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
