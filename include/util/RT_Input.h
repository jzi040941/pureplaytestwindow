#ifndef _H_RT_INPUT_
#define _H_RT_INPUT_

#include "RT_Base.h"
#include <atomic>

#define RECORD_TIME 5
#define __INT 1
struct InputData {
  short *buffer;
  unsigned int bufferBytes;
  unsigned int totalFrames;
  unsigned int frameCounter;
  unsigned int channels;
  std::atomic<int> stock;
};

class RT_Input:public RT_Base {
private:
  // shift frame
  unsigned long buffer_bytes;
  unsigned long frame_counter;
  unsigned int max_stock;

public:

RT_Input(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size);
~RT_Input() { CleanUp(); }

  void GetBuffer(short *);
  void CleanUp();
  int Convert2ShiftedArray(double **arr);
  int Convert2Array(double **arr);

//https://github.com/kooBH/IIP_Demo/issues/212
  int Convert2ShiftedArrayInverted(double **arr);

  InputData data;
};

int rt_call_back(void * /*outputBuffer*/, void *inputBuffer,
                 unsigned int nBufferFrames, double /*streamTime*/,
                 RtAudioStreamStatus /*status*/, void *data);

#endif
