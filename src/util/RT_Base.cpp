#include "util/RT_Base.h"


RT_Base::RT_Base(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size) {
  shift_size = _shift_size;
  sample_rate = _sample_rate;
  device = _device;
  channels = _channels;
  frame_size = _frame_size;
  read_offset = 0;

  ioParams.deviceId = device;
  ioParams.nChannels = channels;
  ioParams.firstChannel = read_offset;
  

  try {
    rtaudio = new RtAudio();
  } catch (RtAudioError &e) {
    std::cout << "ERROR::" << e.getMessage() << '\n' << std::endl;
    CleanUp();
    return;
  }
}

void RT_Base::Start() {
  // startStream 하면 입력스트림으로 소리를 계속 받으면서
  // rt_call_back 을 버퍼가 찰 때마다 (total 아닌 bufferFrame)호출.
  try {
    rtaudio->startStream();
  } catch (RtAudioError &e) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    CleanUp();
  }
  
}

void RT_Base::Stop(){
  
 try {
    rtaudio->stopStream();
  } catch (RtAudioError &e) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    CleanUp();
  }
  
}

void RT_Base::Wait() {

  while (rtaudio->isStreamRunning()) {
    SLEEP(100);
  }
}

bool RT_Base::IsRunnig() {
  if (rtaudio->isStreamRunning())
    return true;
  else
    return false;
}


