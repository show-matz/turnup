//------------------------------------------------------------------------------
//
// Utilities.cxx
//
//------------------------------------------------------------------------------
#include "Utilities.hxx"

//#include "Types.hxx"
//#include "Month.hxx"
//#include "Date.hxx"
//#include "Theme.hxx"
//#include "Line.hxx"
//#include "DataID.hxx"
//#include "StringResource.hxx"
//#include "Environment.hxx"
//#include "Database.hxx"
//#include "RecordHeader.hxx"
//#include "IndexMaster.hxx"

//#include <iostream>
//#include <iomanip>
//#include <string>
//#include <cstring>
//#include <sstream>
//#include <algorithm>

using namespace std;

namespace turnup {

	//--------------------------------------------------------------------------
	//
	// implementation of class Utilities
	//
	//--------------------------------------------------------------------------
	void Utilities::Trim( const char*& p1, const char*& p2 ) {
		for( ; p1 < p2; ++p1 ) {
			if( *p1 != ' ' && *p1 != 0x09 )
				break;
		}
		for( ; p1 < p2; --p2 ) {
			if( p2[-1] != ' ' && p2[-1] != 0x09 )
				break;
		}
	}

//	void Utilities::HexDump( ostream& os, const void* p, uint32_t length ) {
//		constexpr auto addressWidth = sizeof(p) * 2;
//		uint64_t p1 = (uint64_t)p;
//		uint64_t p2 = p1 + length;
//		os << hex << noshowbase << uppercase << setfill('0');
//		uint64_t pX = p1 - (p1 % 16);
//		while( pX < p2 ) {
//			char buf[16+1];
//			os << setw(addressWidth) << pX;
//			for( int i = 0; i < 16; ++i, ++pX ) {
//				if( pX < p1 || p2 <= pX ) {
//					os << Line<3,' '>();
//					buf[i] = ' ';
//				} else {
//					auto c = *reinterpret_cast<uint8_t*>( pX );
//					os << ' ' << setw(2) << static_cast<int>( c );
//					buf[i] = ( isprint( c ) ? c : ' ' );
//				}
//			}
//			buf[16] = 0;
//			os << ' ' << buf << endl;
//		}
//		os << dec;
//	}
//
//	char* Utilities::IsItemLine( char* p1, char* p2, char*& pValueTop ) {
//		// 改行コードを探しつつ、デリミタである : 文字も探す。: がみつかる前に [A-Za-z0-9_/-*] 以外
//		// の文字があればそれは NG。問題なく改行コードまで到達した場合、: の位置と改行コードの位置に
//		// null 文字を設定し、改行コードの次の位置を指すポインタを返す。
//		char* pDelimiter = nullptr;
//		char* pLineEnd   = nullptr;
//		for( char* p = p1; p < p2; ++p ) {
//			char c = *p;
//			if( c == 0x0A ) {
//				pLineEnd = p;
//				break;
//			}
//			if( !pDelimiter ) {
//				if( c == ':' )
//					pDelimiter = p;
//				else if( !(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') ||
//						   ('0' <= c && c <= '9') || c == '_' || c == '/' || c == '-' || c == '*') )
//					break;
//			}
//		}
//		if( !pDelimiter || !pLineEnd )
//			return nullptr;
//		*pDelimiter = 0;
//		*pLineEnd   = 0;
//		pValueTop = pDelimiter + 1;
//		return pLineEnd + 1;
//	}
//
//	bool Utilities::ResolvePath( const std::string& relPath, DataType& type, std::string& group ) {
//		DataType curType  = type;
//		string   curGroup = group;
//		const char* pRelPath = relPath.c_str();
//		while( true ) {
//			const char* pEnd = strchr( pRelPath, '/' );
//			if( !pEnd )
//				pEnd = pRelPath + strlen( pRelPath );
//			if( !ResolvePathImp( pRelPath, pEnd, curType, curGroup ) )
//				return false;
//			if( !*pEnd )
//				break;
//			pRelPath = pEnd + 1;
//		}
//		type  = curType;
//		group = curGroup;
//		return true;
//	}
//
//	std::string Utilities::GenerateLinkC( char c ) {
//		ostringstream ss;
//		ss << theme::link << '[' << c << ']' << theme::reset;
//		return ss.str();
//	}
//
//	std::string Utilities::GenerateLinkN( int n ) {
//		ostringstream ss;
//		ss << theme::link << '[' << n << ']' << theme::reset;
//		return ss.str();
//	}
//
//	std::string Utilities::GenerateLinkS( const char* p ) {
//		ostringstream ss;
//		ss << theme::link << '[' << *p << ']' << theme::reset << p + 1;
//		return ss.str();
//	}
//
//	std::string Utilities::GenerateCalHeader( const Date& dt, unsigned int width ) {
//		ostringstream ss;
//		ss << Month(dt.Month());
//		auto label = ss.str();
//		ss.str( string{} );
//		int len = width - (label.size() + 5);
//		if( len < 0 )
//			len = 0;
//		ss << string( len / 2, ' ' )
//		   << theme::cal_header
//		   << label << ' ' << dt.Year()
//		   << theme::reset
//		   << string( len - (len / 2), ' ' );
//		return ss.str();
//	}
//	
//	void Utilities::WriteTextWithLink( Environment* pENV, std::ostream& os,
//									   const char* pText, uint32_t length ) {
//
//		using RSC = StringResource;
//
//		const char* pTextEnd = pText + length;
//
//		RSC optimize{DID_CHARS};
//
//		while( pText < pTextEnd ) {
//			auto p1 = std::search_n( pText, pTextEnd, 2, '{' );
//			if( p1 == pTextEnd ) {
//				os.write( pText, pTextEnd - pText );
//				break;
//			}
//			auto p2 = std::search_n( p1, pTextEnd, 2, '}' );
//			if( p2 == pTextEnd ) {
//				os.write( pText, pTextEnd - pText );
//				break;
//			}
//			auto p3 = std::find( p1, p2, ':' );
//			string strID;
//			string label;
//			if( p3 == p2 ) {
//				strID = string{ p1 + 2, p2 };
//				label = strID;
//			} else {
//				strID = string{ p1 + 2, p3 };
//				label = string{ p3 + 1, p2 };
//			}
//			DataID id;
//			if( id.Read( pENV, strID.c_str() ) == nullptr )
//				os << CharRange{pText,p2+2};
//			else {
//				auto& db = pENV->Get<Database>();
//				auto& rc = pENV->Get<RecordCache>();
//				auto pRecord = db.FindRecord( id, DataType{}, &rc );
//				if( !pRecord )
//					os << CharRange{pText,p2+2};
//				else {
//					auto idx = pENV->Get<IndexMaster>().AddOnetime( id, pRecord->recType );
//					os << CharRange{pText,p1}
//					   << theme::link << '@' << idx << ':' << label << theme::reset;
//				}
//			}
//			pText = p2 + 2;
//		}
//	}
//
//	template <>
//	bool Utilities::CheckNumeric<false>( const char* p1, const char* p2 ) {
//		//ToDo : オーバーフローの検出
//		if( p1 < p2 && *p1 == '+' )
//			++p1;
//		return std::all_of( p1, p2,
//							[]( char c ) { return '0' <= c && c <= '9'; } );
//	}
//
//	template <>
//	bool Utilities::CheckNumeric<true>( const char* p1, const char* p2 ) {
//		//ToDo : オーバーフローの検出
//		if( p1 < p2 && (*p1 == '-' || *p1 == '+' ) )
//			++p1;
//		return std::all_of( p1, p2,
//							[]( char c ) { return '0' <= c && c <= '9'; } );
//	}

}

