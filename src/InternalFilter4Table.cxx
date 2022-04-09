//------------------------------------------------------------------------------
//
// InternalFilter4Table.cxx
//
//------------------------------------------------------------------------------
#include "InternalFilter4Table.hxx"

#include "Operator4Table.hxx"
#include "TextSpan.hxx"
#include "TextMaker.hxx"
#include "DocumentInfo.hxx"

#include <iostream>
#include <vector>

namespace turnup {

	bool InternalFilter4Table( std::ostream& os, DocumentInfo& docInfo,
							   const TextSpan* pTop, const TextSpan* pEnd ) {
		(void)os;
		std::vector<TextSpan> lines;

		// default の区切り文字はカンマ
		char delim = ','; {
			// デリミタ指定が先頭行にある場合は対応
			TextSpan tmp;
			if( pTop->IsMatch( "delimiter=", tmp, "" ) == true ) {
				if( tmp.IsEqual( "tab" ) )
					delim = 0x09;
				else {
					if( tmp.ByteLength() != 1 )
						std::cerr << "WARNING : delimiter of filter 'table' must be single char."
								  << std::endl;
					delim = *(tmp.Top());
				}
				++pTop;
			}
		}

		for( ; pTop < pEnd; ++pTop ) {
			TextMaker stream;
			TextSpan line = *pTop;
			while( line.IsEmpty() == false ) {
				TextSpan item = line.CutNextToken( delim );
				line.Chomp( 1, 0 );
				stream << "| " << item << " ";
			}
			stream << "|";
			lines.push_back( stream.GetSpan() );
		}

		pTop = &(lines[0]);
		pEnd = pTop + lines.size();
		Operator4Table( pTop, pEnd, docInfo );
		return true;
	}

} // namespace turnup
