#pragma once

#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace nextion {
class NextionSensor;

class NextionSensor : public NextionComponent, public sensor::Sensor, public PollingComponent {
 public:
  NextionSensor(Nextion *nextion) { this->nextion_ = nextion; }

  void process_sensor(uint8_t page_id, uint8_t component_id, float state) override;
  void restore_from_nextion() override;
  void update() override;

 protected:
  Nextion *nextion_;
  bool has_restored_ = false;
};  // namespace nextion
}  // namespace nextion
}  // namespace esphome