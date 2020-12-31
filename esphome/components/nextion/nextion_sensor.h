#pragma once

#include "esphome/components/nextion/nextion.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace nextion {
class NextionSensor;

class NextionSensor : public NextionComponent, public sensor::Sensor, public PollingComponent {
 public:
  NextionSensor(Nextion *nextion) { this->nextion_ = nextion; }
  void process_sensor(char *variable_name, float state) override;
  void update() override;
  void write_state(uint32_t state);
  void nextion_setup() override;

 protected:
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome
