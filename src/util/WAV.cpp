#include "WAV.h"

WAV::WAV() {
#ifndef NDEBUG
//  printf("WAV::contsructor\n");
#endif
  riff_id[0] = 'R';
  riff_id[1] = 'I';
  riff_id[2] = 'F';
  riff_id[3] = 'F';

  wave_id[0] = 'W';
  wave_id[1] = 'A';
  wave_id[2] = 'V';
  wave_id[3] = 'E';

  fmt_id[0] = 'f';
  fmt_id[1] = 'm';
  fmt_id[2] = 't';
  fmt_id[3] = ' ';

  // short 16bit ->2 bytes
  fmt_size = 16;

  // 1- PCM
  fmt_type = 1;

  channels = 0;

  // bit per sample, 8 or 16. not sure
  // Presume short = 16 bit
  bit_per_sample = 16;

  // have to be Optional
  sample_rate = 0;
  // smaple_rate * channels * fmt_size / 8
  byte_rate = sample_rate * channels * fmt_size / 8;

  block_align = bit_per_sample * channels / 8;

  data_id[0] = 'd';
  data_id[1] = 'a';
  data_id[2] = 't';
  data_id[3] = 'a';

  // Number of Samples * Number of Channels * Bit_per_sample / 8
  data_size = 0 * channels * bit_per_sample / 8;

  riff_size = data_size + 44;

  IsOpen = false;
  use_buf = false;
}

WAV::WAV(short _ch, uint32_t _rate) : WAV() {
#ifndef NDEBUG
//  printf("WAV::constructor (ch,rate)\n");
#endif

  channels = _ch;
  // have to be Optional
  sample_rate = _rate;

  // have to be Optional
  // smaple_rate * channels * fmt_size / 8
  byte_rate = sample_rate * channels * fmt_size / 8;

  block_align = bit_per_sample * channels / 8;

  // smaple_rate * channels * fmt_size / 8
  byte_rate = sample_rate * channels * fmt_size / 8;
  // bit_per_sample*channels /8
  block_align = bit_per_sample * channels / 8;
  // Number of Samples * Number of Channels * Bit_per_sample / 8
  data_size = 0 * channels * bit_per_sample / 8;
  riff_size = data_size + 44;
}

WAV::WAV(short _ch, uint32_t _rate, int _frame_size, int _shift_size)
    : WAV(_ch, _rate) {
#ifndef NDEBUG
//  printf("WAV::constructor(ch,rate,frame,shift)\n");
#endif

  frame_size = _frame_size;
  shift_size = _shift_size;
  use_buf = true;

  buf = new short[channels * shift_size];
}

WAV::~WAV() {

  if (IsOpen) {
#ifndef NDEBUG
    //printf("WAV::%s destructor\n", file_name);
#endif
    Finish();
  }
  if (use_buf)
    delete buf;
}

void WAV::WriteHeader() {
  if (!file) {
    std::cout << "ERROR::File doesn't exist\n";
  }
  fseek(file, 0, SEEK_SET);
#ifndef NDEBUG
//  printf("WriteHeader::ftell %ld\n",ftell(file));
#endif
  riff_size = data_size + 44;

  fwrite(riff_id, sizeof(char), 4, file);
  fwrite(&(riff_size), sizeof(uint32_t), 1, file);
  fwrite((wave_id), sizeof(char), 4, file);
  fwrite((fmt_id), sizeof(char), 4, file);
  fwrite(&(fmt_size), sizeof(uint32_t), 1, file);
  fwrite(&(fmt_type), sizeof(short), 1, file);
  fwrite(&(channels), sizeof(short), 1, file);
  fwrite(&(sample_rate), sizeof(uint32_t), 1, file);
  fwrite(&(byte_rate), sizeof(uint32_t), 1, file);
  fwrite(&(block_align), sizeof(short), 1, file);
  fwrite(&(bit_per_sample), sizeof(short), 1, file);
  fwrite(data_id, sizeof(char), 4, file);
  fwrite(&(data_size), sizeof(uint32_t), 1, file);
}

int WAV::NewFile(const char *_file_name) {
  file = fopen(_file_name, "wb");
  if (file == NULL) {
    printf("Failed to Open : %s\n", _file_name);
    return 1;
  }
  WriteHeader();
  file_name = _file_name;
  IsOpen = true;

  return 0;
};

int WAV::Append(short *app_data, UI app_size) {
  fseek(file, 0, SEEK_END);
#ifndef NDEBUG
// printf("Append::ftell %ld | size %d\n",ftell(file),app_size);
#endif
  if (!fwrite(app_data, sizeof(short), app_size, file))
    printf("ERROR::Append");
  data_size += app_size * sizeof(short);
  WriteHeader();
  return 0;
};

int WAV::OpenFile(const char *_file_name) {
  file = fopen(_file_name, "rb");
  if (file == NULL) {
    printf("ERROR::Failed to Open : %s\n", _file_name);
    return 1;
  }
  ReadHeader();
  file_name = _file_name;
  IsOpen = true;


  return 0;
};

void WAV::ReadHeader() {

  unsigned char buffer4[4];
  unsigned char buffer2[2];
  unsigned int temp;
  if (!file) {
    std::cout << "ERROR::File doesn't exist\n";
  }

  fread(riff_id, sizeof(riff_id), 1, file);

  fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endial to big endian 4 bytes int;
  riff_size =
      buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
  // fread(riff_size,sizeof(riff_size),1,file);

  fread(wave_id, sizeof(wave_id), 1, file);

  fread(fmt_id, sizeof(fmt_id), 1, file);

  fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endial to big endian 4 bytes int;
  fmt_size =
      buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

  fread(buffer2, sizeof(buffer2), 1, file);
  // convert little endial to big endian 2 bytes int;
  fmt_type = buffer2[0] | (buffer2[1] << 8);

  fread(buffer2, sizeof(buffer2), 1, file);
  // convert little endial to big endian 2 bytes int;
  
  //Checking
  if(channels!=0){
  temp = buffer2[0] | (buffer2[1] << 8);
    if(temp!=channels){
      printf("ERROR::WAV channels is not expected (%d != %d)\n",channels,temp);
      exit(-1);
    }
  }
  else
    channels = buffer2[0] | (buffer2[1] << 8);

  fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endial to big endian 4 bytes int;
  // Checking
  if(sample_rate!=0){
    temp
      =  buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
    if(temp != sample_rate){
      printf("ERROR::WAV sampe_rate is not expected (%d != %d)\n",sample_rate,temp);  
      exit(-1);
    }

  }   
  else
    sample_rate =
      buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

  fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endial to big endian 4 bytes int;
  byte_rate =
      buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);

  fread(buffer2, sizeof(buffer2), 1, file);
  // convert little endial to big endian 2 bytes int;
  block_align = buffer2[0] | (buffer2[1] << 8);

  fread(buffer2, sizeof(buffer2), 1, file);
  // convert little endial to big endian 2 bytes int;
  bit_per_sample = buffer2[0] | (buffer2[1] << 8);

  fread(data_id, sizeof(data_id), 1, file);

  fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endial to big endian 4 bytes int;
  data_size =
      buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
}

void WAV::ReadUnit(short*dest,int unit){
  fread(dest,sizeof(short),unit,file);
}

void WAV::Finish() {

  if (file) {
    WriteHeader();
    fclose(file);
    IsOpen = false;
  }
}

void WAV::Print() const {

  int t;
  if (!IsOpen)
    return;
  printf("-------- WAV HEADER INFOMATION of [ %s ] ----------\n", file_name);
  printf("riff_id        : %c%c%c%c\n", riff_id[0], riff_id[1], riff_id[2],
         riff_id[3]);
  printf("riff_size      : %u \n", riff_size);
  printf("wave_id        : %c%c%c%c\n", wave_id[0], wave_id[1], wave_id[2],
         wave_id[3]);
  printf("fmt_id         : %c%c%c%c\n", fmt_id[0], fmt_id[1], fmt_id[2],
         fmt_id[3]);
  printf("fmt_size       : %u\n", fmt_size);
  t = fmt_type;
  switch (t) {
  case 1:
    printf("fmt_type       : %u - PCM\n", fmt_type);
    break;
  case 3:
    printf("fmt_type       : %u - IEEE float\n", fmt_type);
    break;
  case 6:
    printf("fmt_type       : %u - 8bit A law\n", fmt_type);
    break;
  case 7:
    printf("fmt_type       : %u - 8 bit U law\n", fmt_type);
    break;
  }
  printf("channels       : %u\n", channels);
  printf("sample_rate    : %u \n", sample_rate);
  printf("byte_rate      : %u\n", byte_rate);
  printf("block_align    : %u\n", block_align);
  printf("bit_per_sample : %u\n", bit_per_sample);
  printf("data_id        : %c%c%c%c\n", data_id[0], data_id[1], data_id[2],
         data_id[3]);
  printf("data_size      : %u\n", data_size);
  printf("duration       : %.3lf sec\n", (double)riff_size / byte_rate);
}

int WAV::IsEOF() const { return feof(file); }

void WAV::Rewind() {
#ifndef NDEBUG
  printf("INFO::Rewind\n");
#endif
  rewind(file);
}

void WAV::Convert2ShiftedArray(double **raw) {
  int i, j,read;
  read=fread(buf, sizeof(short), channels * shift_size, file);
  /*
  printf("READ : %d\n",read);
  for(int q=0;q<shift_size;q++)
    printf("read %d %d\n",q,buf[q]);
*/
  if(read == channels*shift_size){
      // SHIFT
      for (j = 0; j < channels; j++) {
        for (i = 0; i < (frame_size - shift_size); i++) {
          raw[j][i] = raw[j][i + shift_size];
        }
      }
    // COPY as doulbe
    //  memcpy(arr,data.buffer+read_offset,shift_size);
    for (i = 0; i < shift_size; i++) {
      for (j = 0; j < channels; j++){
        raw[j][i + (frame_size - shift_size)] = static_cast<double>(buf[i * channels + j]);
      }
    }
  // Not enough buf to read
  }else{
    read = read/channels;
      for (j = 0; j < channels; j++) {
        for (i = 0; i < (frame_size - shift_size); i++) {
          raw[j][i] = raw[j][i + shift_size];
        }
      }

    for (i = 0; i < read; i++) {
      for (j = 0; j < channels; j++)
        raw[j][i + (frame_size - shift_size)] =static_cast<double>(buf[i * channels + j]);
    }
    for (i = read; i < shift_size; i++) {
      for (j = 0; j < channels; j++)
        raw[j][i + (frame_size - shift_size)] = 0;
    }
  }
}

void WAV::SplitBy2(const char* f1,const char* f2){
  if(!IsOpen){
    printf("ERROR::WAV must be opened\n");
    exit(-1);
  }
  WAV w1(1,sample_rate);
  WAV w2(1,sample_rate);

  w1.NewFile(f1);
  w2.NewFile(f2);

  short temp;

  while(!feof(file)){
    fread(&temp, sizeof(short), 1, file);
    w1.Append(&temp,1);

    fread(&temp, sizeof(short), 1, file);
    w2.Append(&temp,1);
 
  }

  w1.Finish();
  w2.Finish();

}

int WAV::GetChannels(){
  return channels;
}


bool WAV::GetIsOpen(){
  return IsOpen;
}

uint32_t WAV::GetSize(){
  return riff_size;
}


void WAV::UseBuf(int _frame_size,int _shift_size){

  if(!use_buf){
    use_buf = true;
    frame_size = _frame_size;
    shift_size = _shift_size;
    buf = new short[channels * shift_size];
  
  }
}


