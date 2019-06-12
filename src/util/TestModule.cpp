#include "util/TestModule.h"

void TestModule::DoTest(){
  printf("StereoAEC : %lf\n",TestStereoAEC());
  printf("WPE : %lf\n",TestWPE());
}

double TestModule::CheckDiff(const char* output,const char* sample){
  WAV wav_out;
  WAV wav_sample;

  wav_out.OpenFile(output);
  wav_sample.OpenFile(sample);

  double diff = 0;
  int length=0;
  const int unit = 128;

  short buf_output[unit];
  short buf_sample[unit];

  while(!(wav_out.IsEOF() | wav_sample.IsEOF() )){
    wav_out.ReadUnit(buf_output,unit);
    wav_sample.ReadUnit(buf_sample,unit);

  for(int i=0;i<unit;i++)
     diff += fabs(buf_output[i]-buf_sample[i]);
  length++;
  
  }

  return diff/(length*unit);
}

double TestModule::TestStereoAEC(){
  int i,j;
  /*** Init ***/
  int channels = StereoAEC_channels;
  const int reference = 2;
  in = new WAV(channels + reference,sample_rate,frame_size,shift_size);
  out = new WAV(channels,sample_rate);
  raw = new double *[channels + reference];
  data = new double *[channels + reference];
  for (i = 0; i < channels + reference; i++) {
    data[i] = new double[(frame_size + 2)];
    memset(data[i], 0, (frame_size + 2) * sizeof(double));
    raw[i] = new double[frame_size];
    memset(raw[i], 0, (frame_size) * sizeof(double));
  }
  hw = new HannWindow(frame_size, shift_size);
#ifdef _Numerical_FFT_
  fft_in = new Num_FFT(frame_size, channels + reference);
  fft_out = new Num_FFT(frame_size, channels);
#else
  fft_in = new Ooura_FFT(frame_size, channels + reference);
  fft_out = new Ooura_FFT(frame_size, channels);
#endif
  ap = new AfterProcessor(frame_size, shift_size, channels);
  aec = new StereoANC_RLS(frame_size, shift_size, channels);
  /*** Test ***/
  in->OpenFile("samples/7_input_aec.wav");
  out->NewFile("output_stereoAEC.wav");
  while(!in->IsEOF()){
   in->Convert2ShiftedArray(raw);
#pragma omp parallel for
    for (i = 0; i < channels + reference; i++)
      memcpy(data[i], raw[i], sizeof(double) * frame_size);
#pragma omp parallel for private(j)
    for (i = 0; i < channels + reference; i++)
      for (j = 0; j < frame_size; j++)
        data[i][j] /= 32767.0;
    hw->WindowFunc(data, channels + reference);
    fft_in->FFT(data);
    /*** AEC ***/
    aec->Inplace(data);
    fft_out->iFFT(data);
    hw->WindowFunc(data, channels);
    out->Append(ap->Overlap(data), shift_size * channels);
  }
  in->Finish();
  out->Finish();
  /*** Rel ***/
  delete in,out,hw,fft_in,fft_out,ap,aec;
  for (i = 0; i < channels + reference; i++) {
    delete data[i];
    delete raw[i];
  }
  delete data;
  delete raw;
  return CheckDiff("output_stereoAEC.wav","samples/7_output.wav");
}

double TestModule::TestWPE(){
  int i,j;
  /*** Init ***/
  int channels = WPE_channels;
  const int reference = 0;
  in = new WAV(channels + reference,sample_rate,frame_size,shift_size);
  out = new WAV(channels,sample_rate);
  raw = new double *[channels + reference];
  data = new double *[channels + reference];
  for (i = 0; i < channels + reference; i++) {
    data[i] = new double[(frame_size + 2)];
    memset(data[i], 0, (frame_size + 2) * sizeof(double));
    raw[i] = new double[frame_size];
    memset(raw[i], 0, (frame_size) * sizeof(double));
  }
  hw = new HannWindow(frame_size, shift_size);
#ifdef _Numerical_FFT_
  fft_in = new Num_FFT(frame_size, channels + reference);
  fft_out = new Num_FFT(frame_size, channels);
#else
  fft_in = new Ooura_FFT(frame_size, channels + reference);
  fft_out = new Ooura_FFT(frame_size, channels);
#endif
  ap = new AfterProcessor(frame_size, shift_size, channels);
  wpe = new WPE(sample_rate, frame_size, shift_size, channels);
  /*** Test ***/
  in->OpenFile("samples/3_input_wpe.wav");
  out->NewFile("output_wpe.wav");
  while(!in->IsEOF()){
   in->Convert2ShiftedArray(raw);
#pragma omp parallel for
    for (i = 0; i < channels + reference; i++)
      memcpy(data[i], raw[i], sizeof(double) * frame_size);
#pragma omp parallel for private(j)
    for (i = 0; i < channels + reference; i++)
      for (j = 0; j < frame_size; j++)
      data[i][j] /= 32767.0;
    hw->WindowFunc(data, channels + reference);
    fft_in->FFT(data);
    /*** WPE ***/
    wpe->Inplace(data);
    fft_out->iFFT(data);
    hw->WindowFunc(data, channels);
    out->Append(ap->Overlap(data), shift_size * channels);
  }
  in->Finish();
  out->Finish();
  /*** Rel ***/
  delete in,out,hw,fft_in,fft_out,ap,wpe;
  for (i = 0; i < channels + reference; i++) {
    delete data[i];
    delete raw[i];
  }
  delete data;
  delete raw;
  return CheckDiff("output_wpe.wav","samples/3_output.wav");
}




