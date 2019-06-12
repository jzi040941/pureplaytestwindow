#include "RT_Input.h"

RT_Input::RT_Input(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size):
          RT_Base(_device, _channels,_sample_rate,_shift_size,_frame_size){
  max_stock = sample_rate * 3;
#ifndef NDEBUG
  std::cout << "INIT RT_Input ::device:" << device << "\n";
#endif

  /* Unused
    if ( device == 0 )
      ioParams.deviceId = rtaudio->getDefaultInputDevice();
    else
      ioParams.deviceId = device;
  */

  /*** BLOCK 3 ***/
  data.buffer = 0;
  data.stock = 0;

  // input_size 는 shift_size 와 따로 놀것임.
  input_size = shift_size;
  try {
    rtaudio->openStream(NULL, &ioParams, FORMAT, sample_rate, &input_size,
                    &rt_call_back, (void *)&data);
  } catch (RtAudioError &e) {
    std::cout << "ERROR::" << e.getMessage() << '\n' << std::endl;
    CleanUp();
    return;
  }
#ifndef NDEBUG
  printf("input_size : %d\n", input_size);
#endif
  // input_size might be changed;
  data.bufferBytes = input_size * channels * sizeof(short);
#if __INF
  data.totalFrames = max_stock;
#else
  data.totalFrames = (unsigned long)(sample_rate * RECORD_TIME); // * dtime
#endif

  // stock 만큼만 여유분 구성
  data.frameCounter = 0;
  data.channels = channels;
  unsigned long totalBytes;
  totalBytes = data.totalFrames * channels * sizeof(short);

  // Allocate the entire data buffer before starting stream.
  data.buffer = (short *)malloc(totalBytes);
  if (data.buffer == 0) {
    std::cout << "Memory allocation error ... quitting!\n";
    CleanUp();
  }
}

/*
 * 쓰기의 단위와 읽기의 단위가 다를 수 있다. OS 의존.
 * */
int rt_call_back(void * /*outputBuffer*/, void *inputBuffer,
                 unsigned int nBufferFrames, double /*streamTime*/,
                 RtAudioStreamStatus /*status*/, void *data) {
  InputData *iData = (InputData *)data;

  // Simply copy the data to our allocated buffer.
  unsigned int frames = nBufferFrames;

  //이 부분이 input_max와 동일한지 test
  if (iData->frameCounter + nBufferFrames > iData->totalFrames) {
    frames = iData->totalFrames - iData->frameCounter;
    iData->bufferBytes = frames * iData->channels * sizeof(signed short);
  }

  unsigned long offset = iData->frameCounter * iData->channels;
  memcpy(iData->buffer + offset, inputBuffer, iData->bufferBytes);
  iData->frameCounter += frames;
  // stock
  {
    // iData->stock+=frames;
    iData->stock.fetch_add(frames);
  }
//계속 녹음
#if __INF
  if (iData->frameCounter >= iData->totalFrames) {
    iData->frameCounter = 0;
#ifndef NDEBUG
    std::cout << "RT_Input::Buffer Rewind\n";
#endif
  }
//시간제한
#else
  if (iData->frameCounter >= iData->totalFrames)
    return 2;
#endif
  return 0;
}

void RT_Input::GetBuffer(short *raw) {

  memcpy(raw, data.buffer + read_offset, shift_size * channels * sizeof(short));

  read_offset += channels * shift_size;

  if (read_offset >= data.totalFrames * channels) {
    read_offset = 0;
  }
  // data.stock-=shift_size;
  data.stock.fetch_sub(shift_size);
  if (data.stock > max_stock) {
    printf("*******************************************************\n");
    printf("** FAIL:This program is too slow! Fail to catch up!  **\n");
    printf("*******************************************************\n");
    exit(-1);
  }
}

// arr : double[channels][frame_size]
int RT_Input::Convert2ShiftedArray(double **arr) {

#ifndef NDEBUG
// printf("Convert2ShiftedArray::read_offset:%d|stock:%d\n",read_offset,data.stock);
#endif

  // SHIFT
  for (int j = 0; j < channels; j++) {
    for (int i = 0; i < (frame_size - shift_size); i++) {
      arr[j][i] = arr[j][i + shift_size];
    }
  }
  // COPY as doulbe
  //  memcpy(arr,data.buffer+read_offset,shift_size);
  for (int i = 0; i < shift_size; i++) {
    for (int j = 0; j < channels; j++)
      arr[j][i + (frame_size - shift_size)] =
          (double)(data.buffer[i * channels + j + read_offset]);
  }
  read_offset += channels * shift_size;
  // Back to first index
  if (read_offset >= data.totalFrames * channels) {
#ifndef NDEBUG
//    std::cout<<"Convert2Array::Back to first offset\n";
#endif
    read_offset = 0;
  }
  data.stock.fetch_sub(shift_size);
  // FAIL - Real Time
  if (data.stock > max_stock) {
    printf("*******************************************************\n");
    printf("** FAIL:This program is too slow! Fail to catch up!  **\n");
    printf("*******************************************************\n");
    return 1;
  }
  return 0;
}

// arr : double[channels][shift_size]
int RT_Input::Convert2Array(double **arr) {
#ifndef NDEBUG
// printf("Convert2Array::read_offset:%d|stock:%d\n",read_offset,data.stock.load());
#endif

  // COPY as doulbe
  //  memcpy(arr,data.buffer+read_offset,shift_size);
  for (int i = 0; i < shift_size; i++) {
    for (int j = 0; j < channels; j++)
      arr[j][i] = (double)(data.buffer[i * channels + j + read_offset]);
  }
  read_offset += channels * shift_size;
  // Back to first index
  if (read_offset >= data.totalFrames * channels) {
#ifndef NDEBUG
    std::cout << "Convert2Array::Back to first offset\n";
#endif
    read_offset = 0;
  }
  data.stock.fetch_sub(shift_size);
  // FAIL - Real Time
  if (data.stock > max_stock) {
    printf("*******************************************************\n");
    printf("** FAIL:This program is too slow! Fail to catch up!  **\n");
    printf("*******************************************************\n");
    return 1;
  }
  return 0;
}


void RT_Input::CleanUp() {

  if (rtaudio->isStreamOpen())
    rtaudio->closeStream();
  if (data.buffer)
    free(data.buffer);
  if (rtaudio)
    delete rtaudio;
}


int RT_Input::Convert2ShiftedArrayInverted(double **arr){
#ifndef NDEBUG
// printf("Convert2ShiftedArray::read_offset:%d|stock:%d\n",read_offset,data.stock);
#endif
  double * t1;
  /* Swap Pairs */
  for(int i =0; i<channels/2;i++){
   t1 = arr[2*i];
   arr[2*i] = arr[2*i+1];
   arr[2*i+1] = t1;
  }


  // SHIFT
  for (int j = 0; j < channels; j++) {
    for (int i = 0; i < (frame_size - shift_size); i++) {
      arr[j][i] = arr[j][i + shift_size];
    }
  }
  // COPY as doulbe
  // AND invert pairs.
  //  memcpy(arr,data.buffer+read_offset,shift_size);
  for (int i = 0; i < shift_size; i++) {
    for (int j = 0; j < channels; j++)

      arr[j][i + (frame_size - shift_size)] =
          (double)(data.buffer[i * channels + j + read_offset]);
  }
  read_offset += channels * shift_size;
  // Back to first index
  if (read_offset >= data.totalFrames * channels) {
#ifndef NDEBUG
//    std::cout<<"Convert2Array::Back to first offset\n";
#endif
    read_offset = 0;
  }
  data.stock.fetch_sub(shift_size);
  /* Recover Pairs */
  for(int i =0; i<channels/2;i++){
   t1 = arr[2*i];
   arr[2*i] = arr[2*i+1];
   arr[2*i+1] = t1;
  }
  // FAIL - Real Time
  if (data.stock > max_stock) {
    printf("*******************************************************\n");
    printf("** FAIL:This program is too slow! Fail to catch up!  **\n");
    printf("*******************************************************\n");
    return 1;
  }
  return 0;

}



