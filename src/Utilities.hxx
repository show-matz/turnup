//------------------------------------------------------------------------------
//
// Utilities.hxx
//
//------------------------------------------------------------------------------
#ifndef UTILITIES_HXX__
#define UTILITIES_HXX__

//#include "DataType.hxx"
//#include "NumericCast.hxx"

#include <stdint.h>
//#include <memory.h>
//#include <iosfwd>
//#include <type_traits>
//#include <limits>

namespace turnup {

//	class Environment;
//	class Date;
	
	template <int N, typename T> 
	inline constexpr unsigned int count_of( T const (&)[N] ) { 
		return N; 
	} 	
	
	inline bool IsSpaceForward( const char* p ) {
		if( *p == 0x20 || *p == 0x09 || !*p )
			return true;
		auto q = reinterpret_cast<const uint8_t*>( p );
		if( q[0] == 0xE3 && q[1] == 0x80 && q[2] == 0x80 )
			return true;
		return false;
	}
	inline bool IsSpaceBackward( const char* p ) {
		if( *p == 0x20 || *p == 0x09 || !*p )
			return true;
		auto q = reinterpret_cast<const uint8_t*>( p );
		if( q[-2] == 0xE3 && q[-1] == 0x80 && q[0] == 0x80 )
			return true;
		return false;
	}

	//--------------------------------------------------------------------------
	//
	// class Utilities
	//
	//--------------------------------------------------------------------------
	class Utilities {
	public:
		Utilities() = delete;
		Utilities( const Utilities& ) = delete;
		Utilities& operator=( const Utilities& ) = delete;
		~Utilities() = delete;
	public:
		static void Trim( const char*& p1, const char*& p2 );
//	public:
//		static void HexDump( std::ostream& os, const void* p, uint32_t length );
//		static char* IsItemLine( char* p1, char* p2, char*& pValueTop );
//		static bool ResolvePath( const std::string& relPath, DataType& type, std::string& group );
//	public:
//		static std::string GenerateLinkC( char c );
//		static std::string GenerateLinkN( int n );
//		static std::string GenerateLinkS( const char* p );
//		static std::string GenerateCalHeader( const Date& dt, unsigned int width = 0 );
//		static void WriteTextWithLink( Environment* pENV, std::ostream& os,
//									   const char* pText, uint32_t length );
//	public:
//		static inline uint32_t Floor( uint32_t x ) {	// x 以下で最大の２の羃乗を求める
//			x = x | ( x >>  1 );
//			x = x | ( x >>  2 );
//			x = x | ( x >>  4 );
//			x = x | ( x >>  8 );
//			x = x | ( x >> 16 );
//			return x - ( x >> 1 );
//		}
//		static inline uint32_t Ceiling( uint32_t x ) {	// x 以上で最小の２の羃乗を求める
//			x = x - 1;
//			x = x | ( x >>  1 );
//			x = x | ( x >>  2 );
//			x = x | ( x >>  4 );
//			x = x | ( x >>  8 );
//			x = x | ( x >> 16 );
//			return x + 1;
//		}
//	public:
//		static inline bool CheckNumericRange( int64_t n, int64_t min, int64_t max ) {
//			return min <= n && n <= max;
//		}
//		template <typename T>
//		static inline bool CheckNumericRange( int64_t n ) {
//			return CheckNumericRange( n, std::numeric_limits<T>::min(),
//										 std::numeric_limits<T>::max() );
//		}
//	public:
//		template <typename T>
//		static inline bool ScanNumber( const char* p, T& val ) {
//			return ScanNumber( p, p + ::strlen( p ), val );
//		}
//		template <typename T>
//		static inline bool ScanNumber( const char* p1, const char* p2, T& val ) {
//			if( CheckNumeric<std::is_signed<T>::value>( p1, p2 ) == false )
//				return false;
//			val = numeric_cast<T>( p1, p2 );
//			return true;
//		}
//	public:
//		template <typename T>
//		static inline const uint8_t* LoadNumeric( const uint8_t* p1, const uint8_t* p2, T& val ) {
//			constexpr const int byteLen = sizeof(T);
//			if( (p2 - p1) < byteLen )
//				return nullptr;
//			::memcpy( &val, p1, byteLen );
//			return p1 + byteLen;
//		}
//		template <typename T>
//		static inline uint8_t* SaveNumeric( T val, uint8_t* pBuf, uint8_t* pBufEnd ) {
//			constexpr const int byteLen = sizeof(T);
//			if( (pBufEnd - pBuf) < byteLen )
//				return nullptr;
//			::memcpy( pBuf, &val, byteLen );
//			return pBuf + byteLen;
//		}
//	private:
//		template <bool SIGNED>
//		static bool CheckNumeric( const char* p1, const char* p2 );
	};

}

#endif // UTILITIES_HXX__
