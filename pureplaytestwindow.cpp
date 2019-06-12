// AudioPlayWindowTest.cpp : Defines the entry point for the application.
//

#include "RT_Output.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main()
{
	RT_Output* sp;
	sp = new RT_Output(0, 2, 48000, 512, 2048);

	FILE* fp = fopen("LRMonoPhase4.wav", "rb");
	if (!fp) {
		std::cout << "Unable to find";
		return 0;
	}
	void* testbuffer = calloc(7442284, 1);
	//fread(testbuffer, 1, 44, fp);
	fread(testbuffer, 1, 7442284, fp);
	sp->FullBufLoadFloatFirst((char*)testbuffer, 3721142);
	sp->Start();
	

		/*
		sp->WAVLoad();
		sp->Start();
		*/

		while (true) {

	}
		return 0;
}
