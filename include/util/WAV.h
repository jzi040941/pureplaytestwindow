#ifndef _H_WAV_
#define _H_WAV_

#include "top.h"

// Data Type is fixed as short
class WAV {
private:
  // 4bytes fixed size header infomation -> uint32_t
  char riff_id[4];    // riff string
  uint32_t riff_size; // overall size of file in bytes
  char wave_id[4];    // wave string
  char fmt_id[4];     // fmt string with trailing null char
  uint32_t fmt_size;  // length of the format data;
  short
      fmt_type; // format type 1-PCM 3-IEEE float 6- 8bit A law, 7- 8bit ,u law
  unsigned short channels;       // no of channel
  uint32_t sample_rate; // SampleRate(blocks per second)
  uint32_t byte_rate;   // ByteRate = SampleRate * NumChannels * BitsPerSample/8
  short block_align;    // NumChannels * BitsPerSample/8
  short bit_per_sample; // bits per sample, 8 - 8bits, 16-16bits etc
  char data_id[4];      // DATA string or FLLR string
  uint32_t data_size;   // NumSamples * NumChannels * BitsPerSample/8 - size of
                        // the nex chunk that will be read
  FILE *file;
  bool IsOpen;
  const char *file_name;

  // For Input usage only
  short *buf;
  bool use_buf;

  int frame_size;
  int shift_size;

public:
  WAV();
  WAV(short _ch, uint32_t _rate);
  WAV(short _ch, uint32_t _rate, int frame_size, int shift_size);
  ~WAV();
  int NewFile(const char *_file_name);
  int OpenFile(const char *_file_name);
  void WriteHeader();
  void Finish();

  void ReadHeader();
  //유닛 단위로 dest에 wav 버퍼를 읽어준다
  // VS에서 인식을 못한다
   void ReadUnit(short*dest,int unit);
  int IsEOF() const;

  void Print() const;
  void Rewind();

  void Convert2ShiftedArray(double **);

  // Split 2 channel Wav into two 1 channel wav files.
  void SplitBy2(const char* f1,const char* f2);
  int GetChannels();
  bool GetIsOpen();
  uint32_t GetSize(); 
  void UseBuf(int frame_size,int shift_size);
};

#endif
