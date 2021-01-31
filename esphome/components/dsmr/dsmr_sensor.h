#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace espdsmr {
// class DsmrSensor;

class DsmrSensor : public sensor::Sensor, public PollingComponent {
 public:
  void update() override;
  void setup() override;
  void process_sensor(char *variable_name, int state);
  void set_variable_name(std::string variable_name) { variable_name_ = variable_name; }

 protected:
  std::string variable_name_;
  int laststate_;
};
}  // namespace espdsmr
}  // namespace esphome