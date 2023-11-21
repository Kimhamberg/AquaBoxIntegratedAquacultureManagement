#include <OneWire.h>
#include <DallasTemperature.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <ESPDash.h>

enum class Status {Good, Warning, Danger};

class Interval {
  private:
    float lower;
    float upper;
  public:
    Interval(float a, float b): lower{a}, upper{b} {}
    bool includes(float point) const {
      return lower <= point && point <= upper;
    }
};

Status getStatus(Interval goodInterval, Interval warningInterval, float point) {
  if (goodInterval.includes(point)) {
    return Status::Good;
  }
  else if (warningInterval.includes(point)) {
    return Status::Warning;
  }
  else {
    return Status::Danger;
  }
}

char* enumToString(Status status) {
  switch(status) {
    case Status::Good:
      return "success";
    case Status::Warning:
      return "warning";
    case Status::Danger:
      return "danger";
    default:
      return "idle";
  }
}


// Hall effect sensor setup
constexpr uint8_t hallPin = 9;
esp_adc_cal_characteristics_t *chars;
constexpr adc_unit_t adc_num = ADC_UNIT_1;
constexpr adc_atten_t atten = ADC_ATTEN_DB_11;
constexpr adc_bits_width_t bit_width = ADC_WIDTH_BIT_13;
constexpr uint32_t default_vref = 5000;
constexpr adc1_channel_t channel = ADC1_CHANNEL_8;
uint32_t previousVoltage = 0;
unsigned long previousHallMicros = 0;
constexpr uint32_t voltageThreshold = 1000;
constexpr float pitch = 0.5;
float velocity = 0.0;
Interval velocityGoodInterval{1, 5};
Interval velocityWarningInterval{0.5, 7.5};


// Temperature sensor setup
constexpr uint8_t oneWireBus = 8;
constexpr uint8_t resolution = 12;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
DeviceAddress deviceAddress;
unsigned long previousTempMillis = 0;
constexpr unsigned long tempInterval = 750;
float temperature = 0.0;
Interval temperatureGoodInterval{19, 23};
Interval temperatureWarningInterval{17, 25};


// WiFi and WebSocket setup
constexpr char* ssid = "ESP32_AP";
constexpr char* password = "12345678";
AsyncWebServer server(80);
ESPDash dashboard(&server);
Card temperatureCard(&dashboard, TEMPERATURE_CARD, "Temperature", "°C");
Card temperatureStatus(&dashboard, STATUS_CARD, "Temperature Status", "success");
Card velocityCard(&dashboard, GENERIC_CARD, "Velocity");
Card velocityStatus(&dashboard, STATUS_CARD, "Velocity Status", "success");
unsigned long previousServerMillis = 0;
constexpr unsigned long serverInterval = 1000;

float periodToVelocity(const unsigned long period) {
  return 1e6 / (3*period) * pitch;
}

void setup() {
  // Hall effect sensor setup
  pinMode(hallPin, INPUT);
  chars = (esp_adc_cal_characteristics_t*)malloc(sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(adc_num, atten, bit_width, default_vref, chars);
  
  // Temperature sensor setup
  sensors.begin();
  sensors.getAddress(deviceAddress, 0);
  sensors.setResolution(deviceAddress, resolution);

  // WiFi setup
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
}

void loop() {
  // Hall effect sensor reading
  const int reading = adc1_get_raw(channel);
  const uint32_t currentVoltage = esp_adc_cal_raw_to_voltage(reading, chars);
  if (currentVoltage > voltageThreshold && previousVoltage <= voltageThreshold) {
    const unsigned long currentHallMicros = micros();
    const unsigned long period = currentHallMicros - previousHallMicros;
    previousHallMicros = currentHallMicros;
    const float newVelocity = periodToVelocity(period);
    if (abs(newVelocity - velocity) < 10) {
      velocity = newVelocity;
    }
    velocityCard.update(velocity);
  }
  previousVoltage = currentVoltage;

  // Temperature reading
  const unsigned long currentTempMillis = millis();
  if (currentTempMillis - previousTempMillis >= tempInterval) {
    previousTempMillis = currentTempMillis;
    sensors.setWaitForConversion(false);
    sensors.requestTemperatures();
    temperature = sensors.getTempC(deviceAddress);
    temperatureCard.update(temperature);
    sensors.setWaitForConversion(true);
  }

  // Transmit sensor data
  const unsigned long currentServerMillis = millis();
  if (currentServerMillis - previousServerMillis >= serverInterval) {
    previousServerMillis = currentServerMillis;
    temperatureStatus.update("", enumToString(getStatus(temperatureGoodInterval, temperatureWarningInterval, temperature)));
    velocityStatus.update("", enumToString(getStatus(velocityGoodInterval, velocityWarningInterval, velocity)));
    dashboard.sendUpdates();
  }
}
