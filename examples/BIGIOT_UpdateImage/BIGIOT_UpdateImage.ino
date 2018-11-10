#include "OV2640.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "time.h"

#define HTTPS_HOST              "www.bigiot.net"
#define HTTPS_PORT              443
#define BIGIOT_API_KEY          "<Your BIGIOT API Key>"      // Put your API Key here
#define BIGIOT_DEVICE_ID        "<Your BIGIOT Device ID>"
#define BIGIOT_INTERFACE_ID     "<Your BIGIOT Interface ID>"
const char *ssid =              "<Your wifi ssid>";      // Put your SSID here
const char *password =          "<Your wifi password>";  // Put your PASSWORD here
const char *ntpServer =         "pool.ntp.org";
const long gmtOffset_sec =      3600;
const int daylightOffset_sec =  3600;

char *request_content = "--------------------------ef73a32d43e7f04d\r\n"
                        "Content-Disposition: form-data; name=\"data\"; filename=\"%s.jpg\"\r\n"
                        "Content-Type: image/jpeg\r\n\r\n";

char *request_end = "\r\n--------------------------ef73a32d43e7f04d--\r\n";


OV2640 cam;
WiFiClientSecure client;
StaticJsonBuffer<512> jsonBuffer;

void update_image(void)
{
    char status[64] = {0};
    char buf[1024];
    struct tm timeinfo;

    cam.run();

    if (!client.connect(HTTPS_HOST, HTTPS_PORT))
    {
        Serial.println("Connection failed");
        return;
    }

    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        snprintf(buf, sizeof(buf), request_content, String(millis()).c_str());
    }
    else
    {
        strftime(status, sizeof(status), "%Y%m%d%H%M%S", &timeinfo);
        snprintf(buf, sizeof(buf), request_content, status);
    }

    uint32_t content_len = cam.getSize() + strlen(buf) + strlen(request_end);


    String request = "POST /pubapi/uploadImg/did/"BIGIOT_DEVICE_ID"/inputid/"BIGIOT_INTERFACE_ID" HTTP/1.1\r\n";
    request += "Host: www.bigiot.net\r\n";
    request += "User-Agent: TTGO-Camera-Demo\r\n";
    request += "Accept: */*\r\n";
    request += "API-KEY: " + String(BIGIOT_API_KEY) + "\r\n";
    request += "Content-Length: " + String(content_len) + "\r\n";
    request += "Content-Type: multipart/form-data; boundary=------------------------ef73a32d43e7f04d\r\n";
    request += "Expect: 100-continue\r\n";
    request += "\r\n";

    Serial.print(request);
    client.print(request);

    client.readBytesUntil('\r', status, sizeof(status));
    Serial.println(status);
    if (strcmp(status, "HTTP/1.1 100 Continue") != 0)
    {
        Serial.print("Unexpected response: ");
        client.stop();
        return;
    }

    client.print(buf);

    uint8_t *image = cam.getfb();
    size_t size = cam.getSize();
    size_t offset = 0;
    size_t ret = 0;
    while (1)
    {
        ret = client.write(image + offset, size);
        offset += ret;
        size -= ret;
        if (cam.getSize() == offset)
        {
            break;
        }
    }
    client.print(request_end);

    client.find("\r\n");

    bzero(status, sizeof(status));
    client.readBytesUntil('\r', status, sizeof(status));
    if (strncmp(status, "HTTP/1.1 200 OK", strlen("HTTP/1.1 200 OK")))
    {
        Serial.print("Unexpected response: ");
        Serial.println(status);
        client.stop();
        return;
    }

    if (!client.find("\r\n\r\n"))
    {
        Serial.println("Invalid response");
    }

    request = client.readStringUntil('\n');

    char *str = strdup(request.c_str());
    if (!str)
    {
        client.stop();
        return;
    }

    char *start = strchr(str, '{');

    JsonObject &root = jsonBuffer.parseObject(start);
    if (!root.success())
    {
        Serial.println("parse data fail");
        client.stop();
        free(str);
        return;
    }
    if (!strcmp((const char *)root["R"], "1"))
    {
        Serial.println("Update Success");
    }
    free(str);
    client.stop();
}

void setup()
{
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
    camera_config.pixel_format = CAMERA_PF_JPEG;
    camera_config.frame_size = CAMERA_FS_SVGA;

    cam.init(camera_config);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
    update_image();
    delay(30000);
}
