#include "OV2640.h"
#include "esp_log.h"
#include <WiFi.h>
#include <WiFiMulti.h>


OV2640 cam;
WiFiServer server(80);
const char *ssid = "xinyuan";         // Put your SSID here
const char *password = "12345678"; // Put your PASSWORD here


void setup()
{
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }
  camera_config_t camera_config;
  camera_config.ledc_channel = LEDC_CHANNEL_0;
  camera_config.ledc_timer = LEDC_TIMER_0;
  camera_config.pin_d0 = 17;
  camera_config.pin_d1 = 35;
  camera_config.pin_d2 = 34;
  camera_config.pin_d3 = 5;
  camera_config.pin_d4 = 39;
  camera_config.pin_d5 = 18;
  camera_config.pin_d6 = 36;
  camera_config.pin_d7 = 19;
  camera_config.pin_xclk = 27;
  camera_config.pin_pclk = 21;
  camera_config.pin_vsync = 22;
  camera_config.pin_href = 26;
  camera_config.pin_sscb_sda = 25;
  camera_config.pin_sscb_scl = 23;
  camera_config.pin_reset = 15;
  camera_config.xclk_freq_hz = 20000000;

  cam.init(camera_config);

  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(F("."));
  // }
  // Serial.println(F("WiFi connected"));
  // Serial.println("");
  // Serial.println(WiFi.localIP());

  // // Start the server
  // server.on("/capture", HTTP_GET, serverCapture);
  // server.on("/stream", HTTP_GET, serverStream);
  // server.onNotFound(handleNotFound);
  // server.begin();
  // Serial.println(F("Server started"));
}

void loop()
{
  cam.run();
  // server.handleClient();
}
