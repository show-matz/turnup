//------------------------------------------------------------------------------
//
// CRC64.cxx
//
//------------------------------------------------------------------------------
#include "CRC64.hxx"

#include <string.h>

namespace turnup {

	static uint64_t	s_crc64Table[256]	= { 0 };

	static void InitializeCRC64Table( void );
	static void GenerateTag( uint64_t crc, char* pBuf );

	//--------------------------------------------------------------------------
	//
	// implementation of class utility CRC64
	//
	//--------------------------------------------------------------------------
	uint64_t CRC64::Calc( char type,
						  const char* pTop, const char* pEnd, char* pTagBuf ) {

		uint32_t length = ( pEnd ? pEnd - pTop : ::strlen( pTop ) );

		auto CRC64 = []( uint64_t a, uint64_t b ) -> uint64_t {
			uint64_t prev = ( a >> 8 ) & 0x00FFFFFFFFFFFFFFULL;
			uint64_t idx = ( a ^ b ) & 0xFF;
			return ( prev ^ s_crc64Table[idx] ^ 0xFFFFFFFFFFFFFFFFULL );
		};
		InitializeCRC64Table( );

		const uint8_t* pBuf = reinterpret_cast<const uint8_t*>( pTop );
		uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;
		crc = CRC64( crc, type );
		for( uint64_t x = 0; x < length; ++x )
			crc = CRC64( crc, pBuf[x] );
		if( !!pTagBuf )
			GenerateTag( crc, pTagBuf );
		return crc;
	}


	//--------------------------------------------------------------------------
	//
	// local functions
	//
	//--------------------------------------------------------------------------
	static void InitializeCRC64Table( void ) {
	#define CRC64_POLY	0xFEDCBA9876543210ULL
		static bool	s_crc64Initialized	= false;
		if( s_crc64Initialized )
			return;
		uint64_t crc;
		for( uint64_t idx = 0; idx < sizeof(s_crc64Table)/sizeof(s_crc64Table[0]); ++idx ) {
			crc = idx;
			for( int j = 8; j > 0; --j ) {
				if( crc & 1 )
					crc = ( crc >> 1 ) ^ CRC64_POLY;
				else
					crc >>= 1;
			}
		#if 0
			std::cout << std::hex << std::setw(16) << std::setfill('0') << crc << std::endl;
		#endif
			s_crc64Table[idx] = crc;
		}
		s_crc64Initialized = true;
	#undef CRC64_POLY
	}

	static void GenerateTag( uint64_t crc, char* pBuf ) {
		const char* chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		static int radix = 0;
		if( !radix )
			radix = ::strlen( chars );
		auto p = pBuf + 11;
		*p = 0;
		while( pBuf < p ) {
			auto n = crc % radix;
			*--p = chars[n];
			crc = (crc - n) / radix;
		}
	}

} // namespace turnup

