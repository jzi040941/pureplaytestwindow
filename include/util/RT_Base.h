#ifndef _H_RT_BASE_
#define _H_RT_BASE_

#define __INF 1

#include <iostream>
#include "RtAudio.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>
// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  //#include <unistd.h>
  //#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


/*
typedef char MY_TYPE;
#define FORMAT RTAUDIO_SINT8
*/

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE 32767.0
/*
typedef S24 MY_TYPE;
#define FORMAT RTAUDIO_SINT24

typedef signed long MY_TYPE;
#define FORMAT RTAUDIO_SINT32

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
*/

class RT_Base {
  protected:
  /*
   * According to OS, size of input buffer might be
   * different from shift_size;
   * ex) surfacebook -> 288
   * */
  unsigned int input_size;
  unsigned int shift_size;
  unsigned int frame_size;
  unsigned int channels;
  unsigned int device;
  unsigned int sample_rate;
  unsigned int read_offset;
  const int head_size = 44;
  //adc,dac -> rtaudio
  RtAudio* rtaudio;
  //iParams,oParams -> ioParams
  RtAudio::StreamParameters ioParams;

  public:
  RT_Base(unsigned int _device, unsigned int _channels,
           unsigned int _sample_rate, unsigned int _shift_size,
           unsigned int _frame_size);
  virtual ~RT_Base() { }

  virtual void CleanUp() {}
  void Start();
  void Stop();
  // Wait for buffer
  void Wait();
  bool IsRunnig();


};




#endif

