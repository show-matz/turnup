//------------------------------------------------------------------------------
//
// main.cxx
//
//------------------------------------------------------------------------------
#include "InputData.hxx"
#include "DocumentInfo.hxx"
#include "HtmlHeader.hxx"
#include "Operators.hxx"
#include "PreProcessor.hxx"
#include "Parameters.hxx"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <string.h>

#ifndef TURNUP_MAJOR_VERSION
	#define TURNUP_MAJOR_VERSION 0
#endif //TURNUP_MAJOR_VERSION

#ifndef TURNUP_MINOR_VERSION
	#define TURNUP_MINOR_VERSION 0
#endif //TURNUP_MINOR_VERSION

#ifndef TURNUP_DRAFT_VERSION
	#define TURNUP_DRAFT_VERSION 1
#endif //TURNUP_DRAFT_VERSION


using namespace turnup;


struct SysVars {
	TextSpan	appName;	// __APP_NAME
	char		appVer[8];	// __APP_VERSION
	TextSpan	fileName;	// __FILE
	char		date[40];	// __DATE
	char		time[40];	// __TIME
};


static void ShowVersion();
static TextSpan RemovePath( const char* pTop, const char* pEnd = nullptr );
static void SetupSystemVariables( SysVars& sysVars, const char* pAppName );
static void InjectSystemVariables( PreProcessor& pp, const SysVars& sysVars );
static bool InjectParamVariables( PreProcessor& pp, const Parameters& params );

//------------------------------------------------------------------------------
//
// Application entry point - function main.
//
//------------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

	Parameters params;
	if( params.Analyze( argc, argv ) == false ) {
		std::cerr << "ERROR : Invalid parameters." << std::endl;
		return 1;
	}
	if( params.VersionMode() ) {
		ShowVersion();
		return 0;
	}

	SysVars sysVars;

	InputData* pInData = nullptr; {
		TextSpan fileName;
		if( params.GetTargetFile( fileName ) == false ) {
			std::cerr << "ERROR : Input file not specified." << std::endl;
			return 1;
		}
		pInData = InputData::Create( fileName );
		if( !pInData ) {
			std::cerr << "aborted." << std::endl;
			return 1;
		}
		sysVars.fileName = RemovePath( fileName.Top(), fileName.End() );
	}

	SetupSystemVariables( sysVars, argv[0] );

	{
		PreProcessor* pPP = PreProcessor::Create();
		InjectSystemVariables( *pPP, sysVars );
		if( InjectParamVariables( *pPP, params ) == false ) {
			std::cerr << "aborted." << std::endl;
			return 1;
		}
		bool ret = pInData->PreProcess( pPP );
		PreProcessor::Release( pPP );
		if( !ret ) {
			std::cerr << "aborted." << std::endl;
			InputData::Release( pInData ); 
			return 1;	
		}
	}

	DocumentInfo	docInfo;
	pInData->PreScan( docInfo );

	//ToDo : check config... config.entityNumberingDepth.

	std::cout << "<html>" << std::endl;
	docInfo.Get<HtmlHeader>().WriteTo( std::cout, docInfo.Get<Config>() );
	std::cout << "<body>" << std::endl;
	
	Operators operators;

	const TextSpan* pTop = pInData->Begin();
	const TextSpan* pEnd = pInData->End();
	while( pTop < pEnd ) {
		pTop = operators.OperateLines( pTop, pEnd, docInfo );
	}

	std::cout << "</body>" << std::endl
			  << "</html>" << std::endl;

	InputData::Release( pInData ); 
	return 0;
}


//------------------------------------------------------------------------------
//
// local functions
//
//------------------------------------------------------------------------------
static void ShowVersion() {
	std::cout << "turnup version "
			  << TURNUP_MAJOR_VERSION << '.'
			  << std::setw(3) << std::setfill('0') << std::right << TURNUP_MINOR_VERSION;
#if 0 < TURNUP_DRAFT_VERSION
	std::cout << " draft - " << TURNUP_DRAFT_VERSION;
#endif //TURNUP_DRAFT_VERSION
	std::cout << std::endl;

}

static TextSpan RemovePath( const char* pTop, const char* pEnd ) {
	if( !pEnd )
		pEnd = pTop + ::strlen( pTop );
	while( true ) {
		auto p = std::find( pTop, pEnd, '/' );
		if( p == pEnd )
			break;
		pTop = p + 1;
	}
	return TextSpan{ pTop, pEnd };
}

static void SetupSystemVariables( SysVars& sysVars, const char* pAppName ) {
	sysVars.appName = RemovePath( pAppName );
	sprintf( sysVars.appVer, "%d.%03d", TURNUP_MAJOR_VERSION, TURNUP_MINOR_VERSION );
	time_t now;
	time( &now );
	struct tm* pTM = localtime( &now );
	sprintf( sysVars.date, "%04d/%02d/%02d", pTM->tm_year+1900, pTM->tm_mon+1, pTM->tm_mday );
	sprintf( sysVars.time, "%02d:%02d:%02d", pTM->tm_hour, pTM->tm_min, pTM->tm_sec );
}

static void InjectSystemVariables( PreProcessor& pp, const SysVars& sysVars ) {
	pp.RegisterVariable( TextSpan{ "__APP_NAME"    }, sysVars.appName            );
	pp.RegisterVariable( TextSpan{ "__APP_VERSION" }, TextSpan{ sysVars.appVer } );
	pp.RegisterVariable( TextSpan{ "__FILE"        }, sysVars.fileName           );
	pp.RegisterVariable( TextSpan{ "__DATE"        }, TextSpan{ sysVars.date }   );
	pp.RegisterVariable( TextSpan{ "__TIME"        }, TextSpan{ sysVars.time }   );
}

static bool InjectParamVariables( PreProcessor& pp, const Parameters& params ) {
	bool result = true;
	const uint32_t cnt = params.DefinitionCount();
	for( uint32_t i = 0; i < cnt; ++i ) {
		TextSpan ref;
		params.Definition( i, ref );	// ref := '-DSYMBOL(=VALUE)?'
		const char* pTop = ref.Top();
		const char* pEnd = ref.End();
		const char* pDelim = std::find( pTop, pEnd, '=' );
		TextSpan name{ pTop + 2, pDelim };
		TextSpan value{ pDelim == pEnd ? pEnd : pDelim + 1, pEnd };
		if( pp.RegisterVariable( name, value ) == false ) {
			std::cerr << "ERROR : invalid variable name in '" << ref << "'." << std::endl;
			result = false;
		}
	}
	return result;
}
