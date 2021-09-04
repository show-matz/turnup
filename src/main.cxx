//------------------------------------------------------------------------------
//
// main.cxx
//
//------------------------------------------------------------------------------
#include "InputData.hxx"
#include "DocumentInfo.hxx"
#include "HtmlHeader.hxx"
#include "Operators.hxx"

//#include <vector>
//#include <algorithm>
//#include <string>
#include <iostream>
//#include <sstream>
//#include <cstdio>
//#include <cstring>

using namespace std;
using namespace turnup;


//------------------------------------------------------------------------------
//
// Application entry point - function main.
//
//------------------------------------------------------------------------------
int main( int argc, char* argv[] ) {

	(void)argc;
	(void)argv;

	//ToDo : implement : check params...

	InputData* pInData = InputData::Create( argv[1] );
	if( !pInData ) {
		std::cerr << "aborted." << std::endl;
		return 1;
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
