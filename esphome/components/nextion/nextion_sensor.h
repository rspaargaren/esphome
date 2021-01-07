#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "nextion_component.h"
#include "nextion_base.h"

namespace esphome {
namespace nextion {
class NextionSensor;

class NextionSensor : public NextionComponent, public sensor::Sensor, public PollingComponent {
 public:
  NextionSensor(NextionBase *nextion) { this->nextion_ = nextion; }
  void update_component() override { this->update(); }
  void update() override;
  void nextion_setup() override;

  void process_sensor(char *variable_name, int state);
  void set_state(int state);
};
}  // namespace nextion
}  // namespace esphome
