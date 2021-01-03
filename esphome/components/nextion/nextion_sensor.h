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
  void update_component() override { this->update(); }
  void set_state(uint32_t state);
  void update() override;
  void nextion_setup() override;

 protected:
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome
