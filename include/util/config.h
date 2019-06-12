#ifndef _H_GET_CONFIG_
#define _H_GET_CONFIG_

#include "top.h"
#include "json_for_modern_cpp/json.hpp"
using json = nlohmann::json;


class ConfigParam{
private:
public:
  int channel, reference, sample_rate, frame_size, shift_size;
  ConfigParam();
};

class ConfigInput{
  private:
  public:
  bool real_time;
  int device;
  std::string input_file;
  ConfigInput();

};

class ConfigVAD{
public:
  int vad_delay, frame_hang_up, frame_hang_down, noise_frame;
  double vad_threshold;

  ConfigVAD();
};

class ConfigModule{
public:
  bool wpe, aec, srp, mvdr, vad, dcica,iva,gcp;
  ConfigModule();
};

class ConfigOption{
  public :
    bool remove_temp,invert_pair;
    ConfigOption();
};

void get_mic_spec(double **);

#endif
