#pragma once
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace nextion {
class NextionTouchComponent;

class NextionTouchComponent : public NextionComponent, public binary_sensor::BinarySensorInitiallyOff {
 public:
  void process(uint8_t page_id, uint8_t component_id, bool on) override;
  void update_component() override {}
  void nextion_setup() override;
};
}  // namespace nextion
}  // namespace esphome
