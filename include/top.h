#ifndef _H_TOP_
#define _H_TOP_

// due to Windows <minwindef.h>
#define NOMINMAX

#define _Numerical_FFT_


/*****************************
 *  S E T T I N G - mics     *
 *****************************/
#define RECORD_TIME 5
#define GAIN 1

/***************************
 *     Runnig Option       *
 **************************/
#define __DEBUG 1

// Run real time permanetly | as RECORD_TIME
#define __INF 1

#define USE_BLAZE 0

/***************************
 *  L I B R A R I E S      *
 **************************/
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
// serial port
#include <cstdio>
// aec
#include <atomic>

#include <thread>


// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <array>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>




// Types
#include <stdint.h>

#define UI unsigned int
#define UL unsigned long
#define SS signed short

#define _min(a, b) ((a) < (b) ? (a) : (b))
#define _max(a, b) ((a) > (b) ? (a) : (b))

#define M_PI 3.14159265358979323846

#endif
