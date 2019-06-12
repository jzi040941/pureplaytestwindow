#ifndef _H_SOUND_PLAY_
#define _H_SOUND_PLAY__

#include "RT_Base.h"
#include "WAV.h"
#include "CircularBuffer.h"

struct OutputData {
  FILE *fd;
  unsigned int channels;
  long size;
  CircularBuffer* cbuf;
  unsigned int sample_rate;
  unsigned int currentpos;
  const void *fullbuf;
};

class RT_Output:public RT_Base {
private:
  // shift frame
  //unsigned long buffer_bytes;
  //unsigned long frame_counter;
  unsigned int bufferFrames;
public:

  RT_Output(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size);
  ~RT_Output() { CleanUp(); }

  void WAVLoad();
  void JumpHeader();
  void PlayFromMilli(long starttimeInMilliSecond);
  void PlayFromByShift(long shiftCount);
  void PlayFromByPercent(float percent); 
  void Play();
  void RealStop();
  void Suspend();
  //void GetBuffer(short *);
  void CleanUp();
  void BufLoad(const char *buf);
  void FullBufLoad(const char *buf, long bufsize);
  void FullBufLoadFloatFirst(const char *buf, long size);
  void BufAppend(const char *buf);
  void BufAppend(const char *buf, size_t size);
  //int Convert2ShiftedArray(double **arr);
  //int Convert2Array(double **arr);

  OutputData data;
};

void errorCallback( RtAudioError::Type type, const std::string &errorText );

int output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );

int samplerate_convert_output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );

int fullbuf_samplerate_convert_output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );

int float_samplerate_convert_output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );
#endif
