#pragma once
#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "nextion_component.h"
#include "nextion_base.h"

namespace esphome {
namespace nextion {
class NextionBinarySensor;

class NextionBinarySensor : public NextionComponent,
                            public binary_sensor::BinarySensorInitiallyOff,
                            public PollingComponent {
 public:
  NextionBinarySensor(NextionBase *nextion) { this->nextion_ = nextion; }
  void update() override;
  void nextion_setup() override;
  void update_component() override { this->update(); }

  void process_bool(char *variable_name, bool on);
  void process_touch(uint8_t page_id, uint8_t component_id, bool on);

  // Set the components page id for Nextion Touch Component
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  // Set the components component id for Nextion Touch Component
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }

  void set_state(bool state);

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
};
}  // namespace nextion
}  // namespace esphome
