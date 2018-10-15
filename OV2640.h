#ifndef OV2640_H_
#define OV2640_H_

#include <Arduino.h>
#include <pgmspace.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "camera.h"

class OV2640
{
public:
  OV2640(){};
  ~OV2640(){};
  esp_err_t init(camera_config_t config);
  void run(void);
  size_t getSize(void);
  uint8_t *getfb(void);
  int getWidth(void);
  int getHeight(void);
  

private:
  camera_pixelformat_t _pixel_format;
};

#endif //OV2640_H_