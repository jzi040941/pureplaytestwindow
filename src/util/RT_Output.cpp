#include "RT_Output.h"
#include "../../lib/libsamplerate/samplerate.h"

// Two-channel sawtooth wave generator.
RT_Output::RT_Output(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size):
          RT_Base(_device, _channels,_sample_rate,_shift_size,_frame_size){
    data.cbuf = new CircularBuffer(8192);
    data.sample_rate = sample_rate;
          }

void RT_Output::WAVLoad(){
  /* read file */
  data.fd = fopen("piano_stereo_test.wav", "rb");
  if(!data.fd){
    std::cout << "Unable to find or open file!\n";
  }
  else{
    fseek(data.fd, 0, SEEK_END);
    data.size = ftell(data.fd) - head_size;
    std::cout<<data.size<<std::endl;
    fseek(data.fd, 0, SEEK_SET);
  }
  data.sample_rate = sample_rate;
  
 /* 
  //test for resample
  void* testbuffer = malloc(data.size);
  fread(testbuffer, 2, data.size, data.fd);
  void* testoutbuffer = malloc(data.size/2);  
  src_short_to_float_array ((const short*)testbuffer ,(float*) testoutbuffer, data.size);
  SRC_DATA samplerate_data;
  */
   
  // Set Our stream parameters for output only
  bufferFrames = shift_size; //buffer size
  RtAudio::StreamOptions options;
  
  //options.flags = RTAUDIO_ALSA_USE_DEFAULT; 
  options.flags = RTAUDIO_ALSA_USE_DEFAULT|RTAUDIO_HOG_DEVICE;
  data.channels = channels;
 
  if( device == 0)
   ioParams.deviceId = rtaudio->getDefaultOutputDevice();
   
  std::cout << "WAVLoad end. openStream start" << std::endl;
  //adc->openStream(NULL, &ioParams, FORMAT, sample_rate, &input_size, 
  

  try {
   rtaudio->openStream( &ioParams, NULL, FORMAT, sample_rate, &bufferFrames, &output_call_back, (void *)&data,&options );
 }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    CleanUp();
  }
}

void RT_Output::FullBufLoad(const char *buf, long size){
  std::cout<<"FullBufLoad start"<<std::endl; 
  /*
  short * shortbuf = (short *)buf; 
  for(int i=0; i<4000; i++){
    std::cout<<"i:"<<i<<" "<<shortbuf[i]<<std::endl;
  }
  */
  data.fullbuf = (short*)buf;
  bufferFrames = frame_size;
  RtAudio::StreamOptions options;
 
  data.size = size; 
  data.channels = channels;
  ioParams.deviceId = device;
  ioParams.nChannels = channels;
  ioParams.firstChannel = read_offset;
  options.flags = RTAUDIO_ALSA_USE_DEFAULT|RTAUDIO_HOG_DEVICE; 
  

  //data.cbuf->write(buf,512);
  data.currentpos = 0; //same as above write size 
  try {
   rtaudio->openStream( &ioParams, NULL, RTAUDIO_FLOAT32, sample_rate, &bufferFrames, &fullbuf_samplerate_convert_output_call_back, (void *)&data,&options );
 }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    CleanUp();
  }
}

void RT_Output::FullBufLoadFloatFirst(const char *buf, long len){ 
  data.currentpos = 0;
  
  //change to float
  void* float_full_buffer = calloc(len, sizeof(float));
  src_short_to_float_array((short*)buf, (float*)float_full_buffer, len);
  
  
  //set resample params
  SRC_DATA src_data;
  memset (&src_data, 0, sizeof(src_data));
  src_data.src_ratio = data.sample_rate/16000.0; //upsampel ratio for 16000 -> 44100
  src_data.input_frames = len/channels;
  std::cout<<src_data.input_frames<<std::endl;
  src_data.data_in = (float*)float_full_buffer;
  src_data.output_frames = (int)src_data.input_frames*src_data.src_ratio;
  void* resampled_output_buffer = calloc(src_data.output_frames*channels,sizeof(float));
 
  src_data.data_out = (float*)resampled_output_buffer;
 
  
  //resample 
  int error;  
  if ((error = src_simple (&src_data, SRC_ZERO_ORDER_HOLD, channels)))
    
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		exit (1) ;
		} ; 
  data.size = src_data.output_frames; 
  //change to short
  data.fullbuf = calloc(data.size*channels, sizeof(short));
  src_float_to_short_array ((float*)src_data.data_out, (short*)data.fullbuf, data.size*channels) ;
  
  
  //set RT params
  bufferFrames = frame_size;
  RtAudio::StreamOptions options;
  //data.size = size; 
  data.channels = channels;
  ioParams.deviceId = device;
  ioParams.nChannels = channels;
  ioParams.firstChannel = read_offset;
  options.flags = RTAUDIO_ALSA_USE_DEFAULT|RTAUDIO_HOG_DEVICE; 
  
  
  try {
   rtaudio->openStream( &ioParams, NULL, FORMAT, sample_rate, &bufferFrames, &float_samplerate_convert_output_call_back, (void *)&data,&options );
 }
  catch ( RtAudioError& e ) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    CleanUp();
  } 
}

void RT_Output::BufAppend(const char *buf){
  data.cbuf->write(buf,shift_size*channels*sizeof(MY_TYPE));
}

void RT_Output::BufAppend(const char *buf,size_t size){
  data.cbuf->write(buf,size);
}



void RT_Output::PlayFromMilli(long starttimeInMilliSecond){
  fseek(data.fd
      ,(int)starttimeInMilliSecond/1000.0 * channels * sample_rate * sizeof(MY_TYPE)
      ,SEEK_CUR
      );
  Start();
}

void RT_Output::JumpHeader(){
  fseek(data.fd,0,SEEK_SET);
  fseek(data.fd,44,SEEK_CUR); 
}
void RT_Output::PlayFromByShift(long shiftCount){
  JumpHeader();
  fseek(data.fd,shiftCount*shift_size,SEEK_CUR);
  Start();
}

void RT_Output::PlayFromByPercent(float percent){
  JumpHeader();
  fseek(data.fd,int(percent*data.size),SEEK_CUR);
  Start();
 
}

void RT_Output::Play(){
  Start();
   
}

void RT_Output::RealStop(){
  fseek(data.fd,0,SEEK_SET);
  Stop();
}

void RT_Output::Suspend(){
  Stop();
}

void RT_Output::CleanUp() {

  if (rtaudio->isStreamOpen())
    rtaudio->closeStream();
  delete rtaudio;
}

void errorCallback( RtAudioError::Type type, const std::string &errorText )
{
  // This example error handling function does exactly the same thing
  // as the embedded RtAudio::error() function.
  std::cout << "in errorCallback" << std::endl;
  if ( type == RtAudioError::WARNING )
    std::cerr << '\n' << errorText << "\n\n";
  else if ( type != RtAudioError::WARNING )
    throw( RtAudioError( errorText, type ) );
}

//test for samplerate_convert
int samplerate_convert_output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  OutputData *oData = (OutputData*) data;
  
  SRC_DATA src_data;
  memset (&src_data, 0, sizeof(src_data));
  src_data.src_ratio = oData->sample_rate/16000.0; //upsampel ratio for 16000 -> 44100
  std::cout<<"src_ratio"<<src_data.src_ratio<<std::endl;
  src_data.input_frames = (int)floor(oData->channels*nBufferFrames/src_data.src_ratio);
  //change short buffer to float array for resampling
  void* testbuffer=calloc(src_data.input_frames*sizeof(MY_TYPE),1);
  unsigned int count = fread( testbuffer, oData->channels* sizeof( MY_TYPE ), (size_t)nBufferFrames/src_data.src_ratio, oData->fd);
  void* float_buffer = calloc(oData->channels*nBufferFrames,sizeof(float));
  
  src_short_to_float_array((const short*)testbuffer, (float*)float_buffer, oData->channels*nBufferFrames);
  
  //resampling

  src_data.data_in = (float*)float_buffer;
  src_data.output_frames = (int)oData->channels*nBufferFrames;
  src_data.data_out = (float*)calloc(src_data.output_frames,sizeof(float));

  int error;  
  if ((error = src_simple (&src_data, SRC_ZERO_ORDER_HOLD, oData->channels)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		exit (1) ;
		} ;  
  
  //copy resampled frame to outputBuffer 
  memcpy(outputBuffer, (float*)src_data.data_out, src_data.output_frames*sizeof(float));
  
}

int fullbuf_samplerate_convert_output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  OutputData *oData = (OutputData*) data;
  
  //std::cout<<"fullbuf::samplerate callbasck"<<" sameplerate:"<<oData->sample_rate<<std::endl;
  SRC_DATA src_data;
  memset (&src_data, 0, sizeof(src_data));
  src_data.src_ratio = oData->sample_rate/16000.0; //upsampel ratio for 16000 -> 44100
  //std::cout<<"src_ratio"<<src_data.src_ratio<<std::endl;
  src_data.input_frames = (int)oData->channels*(int)floor(nBufferFrames/src_data.src_ratio);
  std::cout<<"src_input_frames:"<<src_data.input_frames<<std::endl;
  //change short buffer to float array for resampling
  void* testbuffer=malloc(src_data.input_frames*sizeof(MY_TYPE));
  /*
   * short* shorttestbuffer = (short*)testbuffer;
  for(int i=5000; i<5100; i++){
    std::cout<<shorttestbuffer[i]<<std::endl;
  }
  */
  oData->cbuf->read((char*)testbuffer, sizeof(MY_TYPE)*src_data.input_frames);
  
  /* 
  void* float_buffer = calloc(oData->channels*nBufferFrames,sizeof(float));
  src_short_to_float_array((const short*)testbuffer, (float*)float_buffer, oData->channels*nBufferFrames);
  */
  
  
  void* float_buffer = calloc(src_data.input_frames,sizeof(float));
  src_short_to_float_array((short*)testbuffer, (float*)float_buffer, src_data.input_frames);
  

  //resampling

  src_data.data_in = (float*)float_buffer;
  src_data.output_frames = (int)oData->channels*nBufferFrames;
  src_data.data_out = (float*)calloc(src_data.output_frames,sizeof(float));

  int error;  
  if ((error = src_simple (&src_data,SRC_SINC_BEST_QUALITY , oData->channels)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		exit(1);
		} ;  
  
  oData->cbuf->write((char*)oData->fullbuf+oData->currentpos, src_data.input_frames*sizeof(MY_TYPE));
  oData->currentpos += src_data.input_frames*sizeof(MY_TYPE);
  if(oData->currentpos > oData->size){
    oData->currentpos = oData->size;
    return 1;
  } 
  //copy resampled frame to outputBuffer 
  memcpy(outputBuffer, (float*)src_data.data_out, src_data.output_frames*sizeof(float));
  }

int float_samplerate_convert_output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  OutputData *oData = (OutputData*) data;
   
  //oData->cbuf->write((char*)oData->fullbuf+oData->currentpos, nBufferFrames*sizeof(float));
  
  memcpy(outputBuffer, (char*)(oData->fullbuf)+oData->currentpos, nBufferFrames*oData->channels*sizeof(short));
  oData->currentpos += nBufferFrames*oData->channels*sizeof(short);
  if(oData->currentpos > oData->size*oData->channels*sizeof(short)){
    oData->currentpos = oData->size*oData->channels*sizeof(short);
    return 1;
  }
  return 0;
}
int output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  OutputData *oData = (OutputData*) data;

  // In general, it's not a good idea to do file input in the audio
  // callback function but I'm doing it here because I don't know the
  // length of the file we are reading.
  
  //file stream example
  
  unsigned int count = fread( outputBuffer, oData->channels * sizeof( MY_TYPE ), nBufferFrames, oData->fd);
  if ( count < nBufferFrames ) {
    unsigned int bytes = (nBufferFrames - count) * oData->channels * sizeof( MY_TYPE );
    unsigned int startByte = count * oData->channels * sizeof( MY_TYPE );
    memset( (char *)(outputBuffer)+startByte, 0, bytes );
    return 1;
  } 
  
  /*
  //used for buffer test
  oData->cbuf->read((char*)outputBuffer, oData->channels * sizeof(MY_TYPE)*nBufferFrames);
  int count = nBufferFrames;
  std::cout<<count<<std::endl;
  if ( (int)count < nBufferFrames ) {
    unsigned int bytes = (nBufferFrames - count) * oData->channels * sizeof( MY_TYPE );
    unsigned int startByte = count * oData->channels * sizeof( MY_TYPE );
    memset( (char *)(outputBuffer)+startByte, 0, bytes );
    return 1;
  }

  //for test
  void* testbuffer=calloc(oData->channels*nBufferFrames*sizeof(MY_TYPE),1);
  fread(testbuffer, oData->channels*sizeof(MY_TYPE), nBufferFrames, oData->fd);
  oData->cbuf->write((char*)testbuffer,(size_t)nBufferFrames*oData->channels*sizeof(MY_TYPE));
  */
  return 0;
}

//usage Example
/*
#include "RT_Output.h"
sp = new RT_Output(device, channels, sample_rate, shift_size, frame_size);
sp = new RT_Output(0,1,16000,128,512);
sp->WAVLoad();
sp->Start();
*/
