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

using namespace turnup;


static void ShowVersion();


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
	}

	{
		PreProcessor* pPP = PreProcessor::Create();
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

#ifndef TURNUP_MAJOR_VERSION
	#define TURNUP_MAJOR_VERSION 0
#endif //TURNUP_MAJOR_VERSION

#ifndef TURNUP_MINOR_VERSION
	#define TURNUP_MINOR_VERSION 0
#endif //TURNUP_MINOR_VERSION

#ifndef TURNUP_DRAFT_VERSION
	#define TURNUP_DRAFT_VERSION 1
#endif //TURNUP_DRAFT_VERSION

	std::cout << "turnup version "
			  << TURNUP_MAJOR_VERSION << '.'
			  << std::setw(3) << std::setfill('0') << std::right << TURNUP_MINOR_VERSION;
#if 0 < TURNUP_DRAFT_VERSION
	std::cout << " draft - " << TURNUP_DRAFT_VERSION;
#endif //TURNUP_DRAFT_VERSION
	std::cout << std::endl;

}
