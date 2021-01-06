#pragma once
#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace nextion {
class NextionBinarySensor;

class NextionBinarySensor : public NextionComponent,
                            public binary_sensor::BinarySensorInitiallyOff,
                            public PollingComponent {
 public:
  NextionBinarySensor(Nextion *nextion) { this->nextion_ = nextion; }
  void process_bool(char *variable_name, bool on) override;
  void update_component() override { this->update(); }
  void update() override;
  void nextion_setup() override;
  void set_state(bool state);

 protected:
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome
