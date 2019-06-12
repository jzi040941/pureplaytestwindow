#ifndef _H_TEST_MODULE_
#define _H_TEST_MODULE_

#include "bottom.h"

class TestModule {

 private:
  char input[64];

  const int sample_rate = 16000;
  const int frame_size = 512;
  const int shift_size = 128;

  const int StereoAEC_channels = 1;
  const int WPE_channels = 6;

  /*************** Essential *****************/
  WAV *in;
  double **raw, **data;
  WAV *out;
  HannWindow *hw;
  FFT_Base *fft_in;
  FFT_Base *fft_out;
  AfterProcessor *ap;


  /*************** Optional *****************/
  StereoANC_RLS *aec;
  WPE *wpe;
  SRP_phat *srp;
  MVDR *mvdr;
  AUX_DCICA *dcica;
  AUX_IVA *iva;
  
  /*************** Tool *********************/
  double TestStereoAEC();
  double TestWPE();

  double CheckDiff(const char* output,const char* sample);

 public:
  /* Empty */
   TestModule(){};
   ~TestModule(){};

   void DoTest();



};

#endif
